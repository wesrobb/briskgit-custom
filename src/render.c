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

typedef struct RenderContext {
    FT_Library fontLibrary;
    FT_Face currentFontFace;
    FrameBuffer frameBuffer;
    float dpiX, dpiY;

    bool initialized;
} RenderContext;

static RenderContext g_renderContext;

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

static void DrawFreeTypeBitmap(FT_Bitmap *bitmap, int32_t x, int32_t y)
{
    Color fontColorLinear = {
        .g = 1.0f,
        .b = 1.0f
    }; // Assumes linear color space not sRGB

    SDL_assert(bitmap);

    FrameBuffer *fb = &g_renderContext.frameBuffer;

    int32_t startX = max(0, x);
    int32_t endX = min(fb->width, x + (int32_t)bitmap->width);

    int32_t startY = max(0, y);
    int32_t endY = min(fb->height, y + (int32_t)bitmap->rows);

    Pixel *destPixels = (Pixel*)fb->pixels;
    destPixels += startX + startY * fb->width;  // Move to the first pixel that resides in the rect
    int32_t nextRow = fb->width - (endX - startX); // Calculate how many pixels to jump over at the end of each row in the rect.

    for (int32_t y = startY, v = 0; y < endY; y++, v++)
    {
        for (int32_t x = startX, u = 0; x < endX; x++, u++)
        {
            float fontAlphaLinear = bitmap->buffer[v * (int32_t)bitmap->width + u] / 255.0f;
            // Pre-multiply font alpha in linear space.
            Color fontColorLinearPremultiplied = {
                .r = fontColorLinear.r * fontAlphaLinear,
                .g = fontColorLinear.g * fontAlphaLinear,
                .b = fontColorLinear.b * fontAlphaLinear,
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

bool Render_Init(int32_t width, int32_t height, float dpiX, float dpiY)
{
    g_renderContext.initialized = true;

    int32_t error = FT_Init_FreeType(&g_renderContext.fontLibrary);
    if (error)
    {
        printf("Failed to init FreeType\n");
        return false;
    }

    error = FT_New_Face(g_renderContext.fontLibrary,
                     "data/Roboto-Regular.ttf",
                     0,
                     &g_renderContext.currentFontFace);
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

    if (FT_HAS_KERNING(g_renderContext.currentFontFace))
    {
        printf("Font has kerning\n");
    }

    Render_Update(width, height, dpiX, dpiY);

    return true;
}

void Render_Destroy()
{
    FT_Done_Face(g_renderContext.currentFontFace);
    FT_Done_Library(g_renderContext.fontLibrary);
}

bool Render_Update(int32_t width, int32_t height, float dpiX, float dpiY)
{
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
    g_renderContext.dpiX = dpiX;
    g_renderContext.dpiY = dpiY;

    return true;
}

void Render_GetDimensions(int32_t *width, int32_t *height)
{
    SDL_assert(g_renderContext.initialized);
    *width = g_renderContext.frameBuffer.width;
    *height = g_renderContext.frameBuffer.height;
}

FrameBuffer *Render_GetFrameBuffer()
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

void Render_DrawRect(Rect rect, Color color)
{
    SDL_assert(g_renderContext.initialized);

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

void Render_DrawFont(int32_t posX, int32_t posY, bool useKerning)
{
    SDL_assert(g_renderContext.initialized);

    FT_Face face = g_renderContext.currentFontFace;

    int32_t error = FT_Set_Char_Size(
          face,    // handle to face object
          0,       // char_width in 1/64th of points
          12*64,   // char_height in 1/64th of points
          (uint32_t)g_renderContext.dpiX,    // horizontal dpi
          (uint32_t)g_renderContext.dpiY);   // vertical dpi
    if (error)
    {
        puts("Failed to set face size");
        return;
    }

    FT_GlyphSlot  slot = face->glyph;  // a small shortcut

    useKerning = FT_HAS_KERNING(face) && useKerning;

    const char* text = "Hello AV.";
    size_t num_chars = strlen(text);
    FT_UInt previous = 0;

    for (size_t n = 0; n < num_chars; n++)
    {
        // convert character code to glyph index
        FT_UInt glyphIndex = FT_Get_Char_Index(face, (FT_ULong)text[n]);

        // retrieve kerning distance and move pen position
        if (useKerning && previous && glyphIndex)
        {
            FT_Vector delta;
            FT_Get_Kerning(face, previous, glyphIndex, FT_KERNING_DEFAULT, &delta);
            posX += delta.x >> 6;
        }

        // load glyph image into the slot (erase previous one)
        error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER);
        if (error)
        {
            printf("Error loading font glyph %d\n - continuing", error);
            continue;  // ignore errors
        }

        // now, draw to our target surface
        DrawFreeTypeBitmap(&slot->bitmap,
                           posX + slot->bitmap_left,
                           posY - slot->bitmap_top);

        // increment pen position
        posX += slot->advance.x >> 6;
        previous = glyphIndex;
    }
}

void Render_DrawFontHarfBuzz(int32_t posX, int32_t posY)
{
    const char *text = "master";
    hb_buffer_t *buf;
    buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text, -1, 0, -1);

    hb_buffer_guess_segment_properties(buf);

    FT_Face face = g_renderContext.currentFontFace;

    int32_t error = FT_Set_Char_Size(
          face,    // handle to face object
          0,       // char_width in 1/64th of points
          12*64,   // char_height in 1/64th of points
          (uint32_t)g_renderContext.dpiX,    // horizontal dpi
          (uint32_t)g_renderContext.dpiY);   // vertical dpi
    if (error)
    {
        puts("Failed to set face size");
        return;
    }
    hb_font_t *font = hb_ft_font_create(face, NULL);

    hb_shape(font, buf, NULL, 0);
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
          error = FT_Load_Glyph(face, glyphid, FT_LOAD_RENDER);
          if (error)
          {
              printf("Error loading font glyph %d\n - continuing", error);
              continue;  // ignore errors
          }

          // now, draw to our target surface
          DrawFreeTypeBitmap(&face->glyph->bitmap,
                  (int32_t)(cursor_x + x_offset),
                  (int32_t)(cursor_y + y_offset - face->glyph->bitmap_top));
          cursor_x += x_advance;
          cursor_y += y_advance;
    }
}
