#include "render.h"

#include <math.h>
#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include <hb.h>
#include <hb-ft.h>

#include "SDL_assert.h"

#include "common.h"

typedef enum RenderCommandType
{
    RENDER_COMMAND_RECT,
    RENDER_COMMAND_FONT
} RenderCommandType;

typedef struct RenderCommandRect
{
    Color color;
} RenderCommandRect;

typedef struct RenderCommandFont
{
    const char *text;
    Color color;
    int32_t ptSize;
} RenderCommandFont;

typedef struct RenderCommand
{
    RenderCommandType type;
    Rect rect;
    union
    {
        RenderCommandRect rectCommand;
        RenderCommandFont fontCommand;
    };
} RenderCommand;

typedef struct RenderContext
{
    FrameBuffer frameBuffer;
    float scaleFactorX, scaleFactorY;

    bool initialized;
} RenderContext;

#define HARFBUZZ_NUM_FEATURES 3
typedef struct FontCache
{
    FT_Library fontLibrary;
    FT_Face faceCache[FONT_COUNT];
    int32_t faceCacheIndex;

    hb_feature_t harfBuzzFeatures[HARFBUZZ_NUM_FEATURES];
} FontCache;

static RenderContext g_renderContext;
static FontCache g_fontCache;

#define RENDER_COMMAND_QUEUE_SIZE 1000
// static RenderCommand g_renderCommandQueue[RENDER_COMMAND_QUEUE_SIZE];
// static int32_t g_renderCommandQueueIndex;

/*
#define MAX_TILE_CACHE_X 100
#define MAX_TILE_CACHE_Y 100
static uint32_t g_tileCache1[MAX_TILE_CACHE_X][MAX_TILE_CACHE_Y];
static uint32_t g_tileCache2[MAX_TILE_CACHE_X][MAX_TILE_CACHE_Y];

static uint32_t g_fnv1aInitial = 2166136261;

// See https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function#FNV-1a_hash
static uint32_t FNV1A(uint32_t initial, unsigned char* buffer, size_t bufferLength)
{
    SDL_assert(buffer);

    uint32_t hash = initial;
    for (size_t i = 0; i < bufferLength; i++)
    {
        hash ^= buffer[i];
        hash *= 16777619;
    }

    return hash;
}
*/

static Pixel ColorToPixel(Color c)
{
    Pixel result = {
        .b = (uint8_t)(c.b * 255.999f),
        .g = (uint8_t)(c.g * 255.999f),
        .r = (uint8_t)(c.r * 255.999f),
        .a = (uint8_t)(c.a * 255.999f),
    };

    return result;
}

static Color PixelToColor(Pixel p)
{
    Color result = {
        .b = p.b / 255.0f,
        .g = p.g / 255.0f,
        .r = p.r / 255.0f,
        .a = p.a / 255.0f,
    };

    return result;
}

static Color SRGBToLinear(Color x)
{
    // Exact gamma value would be 2.2 but we use 2
    // to avoid expensive pow function and use the square of x instead.
    Color result = {
        .r = x.r * x.r,
        .g = x.g * x.g,
        .b = x.b * x.b,
        .a = x.a
    };
    return result;
}

static Color LinearToSRGB(Color x)
{
    Color result = {
        .r = sqrtf(x.r),
        .g = sqrtf(x.g),
        .b = sqrtf(x.b),
        .a = x.a
    };
    return result;
}

// Blends src onto dest assuming linear color space. Src must have pre-multiplied alpha.
static Color LinearBlend(Color src, Color dest)
{
    Color result = {
        .r = src.r + dest.r * (1.0f - src.a),
        .g = src.g + dest.g * (1.0f - src.a),
        .b = src.b + dest.b * (1.0f - src.a),
        .a = dest.a
    };

    return result;
}

static void DrawHollowRect(Rect rect, Color color, int32_t borderThickness)
{
    int32_t startX = max(0, rect.x);
    int32_t endX = min(g_renderContext.frameBuffer.width, rect.x + rect.w);

    // Invert y cos our coordinates are y up but SDL surface is y down.
    int32_t startY = max(0, rect.y);
    int32_t endY = min(g_renderContext.frameBuffer.height, rect.y + rect.h);

    Pixel *pixels = (Pixel*)g_renderContext.frameBuffer.pixels;
    pixels += startX + startY * g_renderContext.frameBuffer.width;  // Move to the first pixel that resides in the rect
    int32_t nextRow = g_renderContext.frameBuffer.width - (endX - startX); // Calculate how many pixels to jump over at the end of each row in the rect.

    Pixel coloredPixel = ColorToPixel(color);
    for (int32_t y = startY; y < endY; y++)
    {
        for (int32_t x = startX; x < endX; x++)
        {
            if ((x < startX + borderThickness || x > endX - borderThickness) ||
                (y < startY + borderThickness || y > endY - borderThickness))
            {
                *pixels = coloredPixel;
            }
            pixels++;
        }

        pixels += nextRow;
    }
}

static void DrawFreeTypeBitmap(FT_Bitmap *bitmap, double x, double y, Color c)
{
    SDL_assert(bitmap);

    FrameBuffer *fb = &g_renderContext.frameBuffer;

    int32_t startX = max(0, (int32_t)(x + 0.5));
    int32_t endX = min(fb->width, (int32_t)(x + bitmap->width + 0.5));

    int32_t startY = max(0, (int32_t)(y + 0.5));
    int32_t endY = min(fb->height, (int32_t)(y + bitmap->rows + 0.5));

    Pixel *destPixels = (Pixel*)fb->pixels;
    destPixels += startX + startY * fb->width;  // Move to the first pixel that resides in the rect
    int32_t nextRow = fb->width - (endX - startX); // Calculate how many pixels to jump over at the end of each row in the rect.

    Rect r = {
        .x = startX,
        .y = startY,
        .w = endX - startX,
        .h = endY - startY
    };
    DrawHollowRect(r, c, 1);

    for (int32_t y = startY, v = 0; y < endY; y++, v++)
    {
        for (int32_t x = startX, u = 0; x < endX; x++, u++)
        {
            float fontAlphaLinear = bitmap->buffer[v * (int32_t)bitmap->width + u] / 255.0f;
            // Pre-multiply font alpha in linear space.
            Color fontColorLinearPremultiplied = {
                .r = c.r * fontAlphaLinear,
                .g = c.g * fontAlphaLinear,
                .b = c.b * fontAlphaLinear,
                .a = fontAlphaLinear
            };

            Color destColorSRGB = PixelToColor(*destPixels);
            Color destColorLinear = SRGBToLinear(destColorSRGB);
            Color blendedLinear = LinearBlend(fontColorLinearPremultiplied, destColorLinear);
            Color finalColorSRGB = LinearToSRGB(blendedLinear);

            *destPixels = ColorToPixel(finalColorSRGB);
            destPixels++;
        }

        destPixels += nextRow;
    }
}

static void ScaleRect(Rect *r)
{
    r->x = (int32_t)(r->x * g_renderContext.scaleFactorX);
    r->w = (int32_t)(r->w * g_renderContext.scaleFactorX);
    r->y = (int32_t)(r->y * g_renderContext.scaleFactorY);
    r->h = (int32_t)(r->h * g_renderContext.scaleFactorY);
}

static const char *GetFontFile(Font font)
{
    // TODO: Bake these files into the exe.
    switch (font)
    {
        case FONT_ROBOTO_REGULAR:
            return "data/Roboto-Regular.ttf";
        case FONT_MENLO_REGULAR:
            return "data/MenloPowerline.ttf";
        default:
            return NULL;
    }
}

bool Render_Init(int32_t width, int32_t height, float scaleFactorX, float scaleFactorY)
{
    g_renderContext.initialized = true;

    g_fontCache.harfBuzzFeatures[0].tag = HB_TAG('k','e','r','n');
    g_fontCache.harfBuzzFeatures[0].value = 1;
    g_fontCache.harfBuzzFeatures[0].start = 0;
    g_fontCache.harfBuzzFeatures[0].end = (uint32_t)-1;
    g_fontCache.harfBuzzFeatures[1].tag = HB_TAG('l','i','g','a');
    g_fontCache.harfBuzzFeatures[1].value = 1;
    g_fontCache.harfBuzzFeatures[1].start = 0;
    g_fontCache.harfBuzzFeatures[1].end = (uint32_t)-1;
    g_fontCache.harfBuzzFeatures[2].tag = HB_TAG('c','l','i','g');
    g_fontCache.harfBuzzFeatures[2].value = 1;
    g_fontCache.harfBuzzFeatures[2].start = 0;
    g_fontCache.harfBuzzFeatures[2].end = (uint32_t)-1;

    int32_t error = FT_Init_FreeType(&g_fontCache.fontLibrary);
    if (error)
    {
        printf("Failed to init FreeType\n");
        return false;
    }

    for (int32_t i = 0; i < FONT_COUNT; i++)
    {
        const char* fontFile = GetFontFile((Font)i);
        error = FT_New_Face(g_fontCache.fontLibrary, fontFile, 0, &g_fontCache.faceCache[i]);
        if (error == FT_Err_Unknown_File_Format)
        {
            puts("Unknown font format");
            return false;
        }
        else if (error)
        {
            printf("Unknown error %d\n", error);
            return false;
        }
    }

    Render_Update(width, height, scaleFactorX, scaleFactorY);

    return true;
}

void Render_Destroy()
{
    SDL_assert(g_renderContext.initialized);

    if (g_renderContext.frameBuffer.pixels)
    {
        free(g_renderContext.frameBuffer.pixels);
        g_renderContext.frameBuffer.pixels = NULL;
    }

    for (int32_t i = 0; i < FONT_COUNT; i++)
    {
        FT_Done_Face(g_fontCache.faceCache[i]);
    }
    FT_Done_Library(g_fontCache.fontLibrary);
}

bool Render_Update(int32_t width, int32_t height, float scaleFactorX, float scaleFactorY)
{
    SDL_assert(g_renderContext.initialized);

    if (g_renderContext.frameBuffer.pixels)
    {
        free(g_renderContext.frameBuffer.pixels);
    }

    g_renderContext.frameBuffer.pixels = malloc((size_t)(width * height) * sizeof(Pixel));
    if (!g_renderContext.frameBuffer.pixels)
    {
        g_renderContext.initialized = false;
        printf("Failed to allocate pixel buffer\n");
        return false;
    }

    g_renderContext.frameBuffer.width = width;
    g_renderContext.frameBuffer.height = height;
    g_renderContext.scaleFactorX = scaleFactorX;
    g_renderContext.scaleFactorY = scaleFactorY;

    return true;
}

void Render_GetDimensions(int32_t *width, int32_t *height, float *scaleFactorX, float *scaleFactorY)
{
    SDL_assert(g_renderContext.initialized);
    *width = g_renderContext.frameBuffer.width;
    *height = g_renderContext.frameBuffer.height;
    *scaleFactorX = g_renderContext.scaleFactorX;
    *scaleFactorY = g_renderContext.scaleFactorY;
}

void Render_BeginFrame()
{
    SDL_assert(g_renderContext.initialized);
}

FrameBuffer *Render_EndFrame()
{
    SDL_assert(g_renderContext.initialized);

    return &g_renderContext.frameBuffer;
}

void Render_Clear(Color color)
{
    SDL_assert(g_renderContext.initialized);

    Pixel coloredPixel = ColorToPixel(color);
    int32_t size = g_renderContext.frameBuffer.height * g_renderContext.frameBuffer.width;
    Pixel *pixels = (Pixel*)g_renderContext.frameBuffer.pixels;
    for (int32_t y = 0; y < size; y++)
    {
        *pixels = coloredPixel;
        pixels++;
    }
}

void Render_DrawRect(Rect rect, Color color) // TODO: These should be passed by pointer
{
    SDL_assert(g_renderContext.initialized);

    ScaleRect(&rect);

    int32_t startX = max(0, rect.x);
    int32_t endX = min(g_renderContext.frameBuffer.width, rect.x + rect.w);

    // Invert y cos our coordinates are y up but SDL surface is y down.
    int32_t startY = max(0, rect.y);
    int32_t endY = min(g_renderContext.frameBuffer.height, rect.y + rect.h);

    Pixel *pixels = (Pixel*)g_renderContext.frameBuffer.pixels;
    pixels += startX + startY * g_renderContext.frameBuffer.width;  // Move to the first pixel that resides in the rect
    int32_t nextRow = g_renderContext.frameBuffer.width - (endX - startX); // Calculate how many pixels to jump over at the end of each row in the rect.

    Pixel coloredPixel = ColorToPixel(color);
    for (int32_t y = startY; y < endY; y++)
    {
        for (int32_t x = startX; x < endX; x++)
        {
            *pixels = coloredPixel;
            pixels++;
        }

        pixels += nextRow;
    }
}

void Render_DrawHollowRect(Rect rect, Color color, int32_t borderThickness) // TODO: These should be passed by pointer
{
    SDL_assert(g_renderContext.initialized);

    ScaleRect(&rect);
    DrawHollowRect(rect, color, borderThickness);
}

void Render_DrawFont(Font font, const char *text, int32_t posX, int32_t posY, int32_t ptSize, Color c)
{
    posX = (int32_t)(posX * g_renderContext.scaleFactorX);
    posY = (int32_t)(posY * g_renderContext.scaleFactorY);

    hb_buffer_t *buf;
    buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text, -1, 0, -1);

    hb_buffer_guess_segment_properties(buf);

    FT_Face face = g_fontCache.faceCache[font];

    int32_t error = FT_Set_Char_Size(
          face,    // handle to face object
          0,       // char_width in 1/64th of points
          ptSize*64,   // char_height in 1/64th of points
          (uint32_t)(72.0f * g_renderContext.scaleFactorX),    // horizontal dpi
          (uint32_t)(72.0f * g_renderContext.scaleFactorY));   // vertical dpi

    if (error)
    {
        puts("Failed to set face size");
        return;
    }
    hb_font_t *hbFont = hb_ft_font_create_referenced(face);
    hb_ft_font_set_load_flags(hbFont, FT_LOAD_NO_HINTING);

    hb_shape(hbFont, buf, g_fontCache.harfBuzzFeatures, HARFBUZZ_NUM_FEATURES);
    uint32_t glyphCount = 0;
    hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(buf, &glyphCount);
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyphCount);

    double cursor_x = posX;
    double cursor_y = posY;

    for (uint32_t i = 0; i < glyphCount; ++i) {
          hb_codepoint_t glyphid = glyph_info[i].codepoint;
          double x_offset = glyph_pos[i].x_offset / 64.0;
          double y_offset = glyph_pos[i].y_offset / 64.0;
          double x_advance = glyph_pos[i].x_advance / 64.0;
          double y_advance = glyph_pos[i].y_advance / 64.0;
          //draw_glyph(glyphid, cursor_x + x_offset, cursor_y + y_offset);
          // load glyph image into the slot (erase previous one)
          error = FT_Load_Glyph(face, glyphid, FT_LOAD_NO_HINTING);
          if (error)
          {
              printf("Error loading font glyph %d\n - continuing", error);
              continue;  // ignore errors
          }
          error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
          if (error)
          {
              printf("Error rendering font glyph %d\n - continuing", error);
              continue;  // ignore errors
          }

          // now, draw to our target surface
          double x = cursor_x + x_offset + face->glyph->bitmap_left;
          double y = cursor_y + y_offset - face->glyph->bitmap_top;
          DrawFreeTypeBitmap(&face->glyph->bitmap, x, y, c);
          cursor_x += x_advance;
          cursor_y += y_advance;
    }

    hb_buffer_destroy(buf);
}

int32_t Render_GetTextWidth(Font font, const char* text, int32_t ptSize)
{
    (void)font;
    (void)text;
    (void)ptSize;
    return 0;
}

int32_t Render_GetTextHeight(Font font, const char* text, int32_t ptSize)
{
    (void)font;
    (void)text;
    (void)ptSize;
    return 0;
}
