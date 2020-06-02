#include "render.h"

#include <assert.h>
#include <math.h>
#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include <hb-ft.h>
#include <hb.h>

#include "eva/eva.h"

#include "common.h"
#include "profiler.h"

typedef enum RenderCommandType { RENDER_COMMAND_RECT, RENDER_COMMAND_FONT } RenderCommandType;

typedef struct RenderCommandRect {
    Color color;
} RenderCommandRect;

typedef struct RenderCommandFont {
    Font font;
    const char *text;
    Color color;
    int32_t ptSize;
    int32_t x, y;
} RenderCommandFont;

typedef struct RenderCommand {
    RenderCommandType type;
    Rect rect;
    union {
        RenderCommandRect rectCommand;
        RenderCommandFont fontCommand;
    };
} RenderCommand;

#define HARFBUZZ_NUM_FEATURES 3
typedef struct FontCache {
    FT_Library fontLibrary;
    FT_Face faceCache[FONT_COUNT];
    hb_font_t *hbFontCache[FONT_COUNT];
    int32_t ptSizes[FONT_COUNT];
    float scale_x[FONT_COUNT];
    float scale_y[FONT_COUNT];

    hb_feature_t harfBuzzFeatures[HARFBUZZ_NUM_FEATURES];
} FontCache;

static FontCache g_fontCache;

#define RENDER_COMMAND_QUEUE_SIZE 1000
typedef struct RenderCommands {
    RenderCommand commands1[RENDER_COMMAND_QUEUE_SIZE];
    RenderCommand commands2[RENDER_COMMAND_QUEUE_SIZE];
    int32_t commandsIndex1;
    int32_t commandsIndex2;

    RenderCommand *current;
    RenderCommand *previous;
    int32_t *currentIndex;
    int32_t *previousIndex;
} RenderCommands;
static RenderCommands g_renderCommands;

//#define MAX_TILE_CACHE_X 100
//#define MAX_TILE_CACHE_Y 100
// static uint32_t g_tileCache1[MAX_TILE_CACHE_X][MAX_TILE_CACHE_Y];
// static uint32_t g_tileCache2[MAX_TILE_CACHE_X][MAX_TILE_CACHE_Y];
// static uint32_t *g_currentTileCache;
//
// static uint32_t g_fnv1aInitial = 2166136261;
//
//// See https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function#FNV-1a_hash
// static uint32_t FNV1A(uint32_t initial, unsigned char* buffer, size_t
// bufferLength)
//{
//    SDL_assert(buffer);
//
//    uint32_t hash = initial;
//    for (size_t i = 0; i < bufferLength; i++)
//    {
//        hash ^= buffer[i];
//        hash *= 16777619;
//    }
//
//    return hash;
//}

static bool TextEquals(const char *a, const char *b)
{
    uint64_t lenA = strlen(a);
    uint64_t lenB = strlen(b);
    if (lenA == lenB) {
        for (uint64_t i = 0; i < lenA; i++) {
            if (a[i] != b[i]) {
                return false;
            }
        }
    }

    return true;
}

static bool RenderCommandsEqual(RenderCommand *a, RenderCommand *b)
{
    // TODO: Compare performance with memcmp.
    // It is more likely the rect changes than the type changes
    // since rendering order is unlikely to vary much.
    // So check the rect first.
    if (a->rect.x == b->rect.x && a->rect.y == b->rect.y && a->rect.w == b->rect.w &&
        a->rect.h == b->rect.h) {
        if (a->type == b->type) {
            switch (a->type) {
            case RENDER_COMMAND_RECT:
                return a->rectCommand.color.a == b->rectCommand.color.a &&
                       a->rectCommand.color.b == b->rectCommand.color.b &&
                       a->rectCommand.color.g == b->rectCommand.color.g &&
                       a->rectCommand.color.r == b->rectCommand.color.r;
            case RENDER_COMMAND_FONT:
                return a->fontCommand.color.a == b->fontCommand.color.a &&
                       a->fontCommand.color.b == b->fontCommand.color.b &&
                       a->fontCommand.color.g == b->fontCommand.color.g &&
                       a->fontCommand.color.r == b->fontCommand.color.r &&
                       a->fontCommand.ptSize == b->fontCommand.ptSize &&
                       TextEquals(a->fontCommand.text, b->fontCommand.text);
            }
        }
    }

    return false;
}

static eva_pixel ColorToPixel(Color c)
{
    eva_pixel result = {
        .b = (uint8_t)(c.b * 255.999f),
        .g = (uint8_t)(c.g * 255.999f),
        .r = (uint8_t)(c.r * 255.999f),
        .a = (uint8_t)(c.a * 255.999f),
    };

    return result;
}

static Color PixelToColor(eva_pixel *p)
{
    Color result = {
        .b = p->b / 255.0f,
        .g = p->g / 255.0f,
        .r = p->r / 255.0f,
        .a = p->a / 255.0f,
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
        .a = x.a };
    return result;
}

// Blends src onto dest assuming linear color space. Src must have
// pre-multiplied alpha.
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

/*
static void DrawHollowRectUnscaled(Rect rect, Color color,
                                   int32_t borderThickness) {
  Profiler_Begin;

  int32_t startX = max(0, rect.x);
  int32_t endX = min(g_renderContext.frameBuffer.width, rect.x + rect.w);

  // Invert y cos our coordinates are y up but SDL surface is y down.
  int32_t startY = max(0, rect.y);
  int32_t endY = min(g_renderContext.frameBuffer.height, rect.y + rect.h);

  Pixel *pixels = (Pixel *)g_renderContext.frameBuffer.pixels;
  pixels +=
      startX +
      startY * g_renderContext.frameBuffer
                   .width; // Move to the first pixel that resides in the rect
  int32_t nextRow = g_renderContext.frameBuffer.width -
                    (endX - startX); // Calculate how many pixels to jump over
                                     // at the end of each row in the rect.

  Pixel coloredPixel = ColorToPixel(color);
  for (int32_t y = startY; y < endY; y++) {
    for (int32_t x = startX; x < endX; x++) {
      if ((x < startX + borderThickness || x > endX - borderThickness) ||
          (y < startY + borderThickness || y > endY - borderThickness)) {
        *pixels = coloredPixel;
      }
      pixels++;
    }

    pixels += nextRow;
  }

  Profiler_End;
} */

static void DrawFreeTypeBitmap(FT_Bitmap *bitmap,
                               double x,
                               double y,
                               Color c,
                               eva_pixel *framebuffer,
                               int32_t framebuffer_width,
                               int32_t framebuffer_height)
{
    Profiler_Begin;

    assert(bitmap);

    int32_t startX = max(0, (int32_t)(x + 0.5));
    int32_t endX = min(framebuffer_width, (int32_t)(x + bitmap->width + 0.5));

    int32_t startY = max(0, (int32_t)(y + 0.5));
    int32_t endY = min(framebuffer_height, (int32_t)(y + bitmap->rows + 0.5));

    // Move to the first pixel that resides in the rect
    framebuffer += startX + startY * framebuffer_width;
    // Calculate how many pixels to jump over at
    // the end of each row in the rect.
    int32_t nextRow = framebuffer_width - (endX - startX);

    for (int32_t y = startY, v = 0; y < endY; y++, v++) {
        for (int32_t x = startX, u = 0; x < endX; x++, u++) {
            float fontAlphaLinear = bitmap->buffer[v * (int32_t)bitmap->width + u] / 255.0f;
            // Pre-multiply font alpha in linear space.
            Color fontColorLinearPremultiplied = { .r = c.r * fontAlphaLinear,
                                                   .g = c.g * fontAlphaLinear,
                                                   .b = c.b * fontAlphaLinear,
                                                   .a = fontAlphaLinear };

            Color destColorSRGB = PixelToColor(framebuffer);
            Color destColorLinear = SRGBToLinear(destColorSRGB);
            Color blendedLinear = LinearBlend(fontColorLinearPremultiplied, destColorLinear);
            Color finalColorSRGB = LinearToSRGB(blendedLinear);

            *framebuffer = ColorToPixel(finalColorSRGB);
            framebuffer++;
        }

        framebuffer += nextRow;
    }

    Profiler_End;
}

static void ScaleRect(Rect *r, float scale_x, float scale_y)
{
    r->x = (int32_t)(r->x * scale_x);
    r->w = (int32_t)(r->w * scale_x);
    r->y = (int32_t)(r->y * scale_y);
    r->h = (int32_t)(r->h * scale_y);
}

static const char *GetFontFile(Font font)
{
    // TODO: Bake these files into the exe.
    switch (font) {
    case FONT_ROBOTO_REGULAR:
        return "data/Roboto-Regular.ttf";
    case FONT_MENLO_REGULAR:
        return "data/MenloPowerline.ttf";
    default:
        return NULL;
    }
}

bool float_eq(float a, float b, float epsilon)
{
    return fabsf(a - b) < epsilon;
}

static void LoadCachedFont(
    Font font, FT_Face *face, hb_font_t **hbFont, int32_t ptSize, float scale_x, float scale_y)
{
    // Load cached font data and only set size if it wasn't set already.
    *face = g_fontCache.faceCache[font];
    *hbFont = g_fontCache.hbFontCache[font];
    if (ptSize != g_fontCache.ptSizes[font] ||
        !float_eq(scale_x, g_fontCache.scale_x[font], 0.1f) ||
        !float_eq(scale_y, g_fontCache.scale_y[font], 0.1f)) {
        int32_t error = FT_Set_Char_Size(*face,       // handle to face object
                                         0,           // char_width in 1/64th of points
                                         ptSize * 64, // char_height in 1/64th of points
                                         (uint32_t)(72.0f * scale_x),  // horizontal dpi
                                         (uint32_t)(72.0f * scale_y)); // vertical dpi
        // int32_t error = FT_Set_Pixel_Sizes(face, 20, 20);

        if (error) {
            puts("Failed to set face size");
            return;
        }
        hb_ft_font_changed(*hbFont);
        g_fontCache.ptSizes[font] = ptSize;
        g_fontCache.scale_x[font] = scale_x;
        g_fontCache.scale_y[font] = scale_y;
    }
}

void DrawRect(Rect rect,
              RenderCommandRect *cmd,
              eva_pixel *framebuffer,
              int32_t framebuffer_width,
              int32_t framebuffer_height,
              float scale_x,
              float scale_y)
{
    Profiler_Begin;

    ScaleRect(&rect, scale_x, scale_y);

    int32_t startX = max(0, rect.x);
    int32_t endX = min(framebuffer_width, rect.x + rect.w);

    // Invert y cos our coordinates are y up but SDL surface is y down.
    int32_t startY = max(0, rect.y);
    int32_t endY = min(framebuffer_height, rect.y + rect.h);

    // Move to the first pixel that resides in the rect
    framebuffer += startX + startY * framebuffer_width;

    /// Calculate how many pixels to jump over
    // at the end of each row in the rect.
    int32_t nextRow = framebuffer_width - (endX - startX);

    eva_pixel coloredPixel = ColorToPixel(cmd->color);
    for (int32_t y = startY; y < endY; y++) {
        for (int32_t x = startX; x < endX; x++) {
            *framebuffer = coloredPixel;
            framebuffer++;
        }

        framebuffer += nextRow;
    }

    Profiler_End;
}

void DrawFont(Rect rect,
              RenderCommandFont *cmd,
              eva_pixel *framebuffer,
              int32_t framebuffer_width,
              int32_t framebuffer_height,
              float scale_x,
              float scale_y)
{
    (void)rect;

    Profiler_Begin;

    cmd->x = (int32_t)(cmd->x * scale_x);
    cmd->y = (int32_t)(cmd->y * scale_y);

    hb_buffer_t *buf;
    buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, cmd->text, -1, 0, -1);
    hb_buffer_guess_segment_properties(buf);

    FT_Face face;
    hb_font_t *hbFont;
    LoadCachedFont(cmd->font, &face, &hbFont, cmd->ptSize, scale_x, scale_y);

    hb_shape(hbFont, buf, g_fontCache.harfBuzzFeatures, HARFBUZZ_NUM_FEATURES);
    uint32_t glyphCount = 0;
    hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(buf, &glyphCount);
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyphCount);

    double cursor_x = cmd->x;
    double cursor_y = cmd->y;

    for (uint32_t i = 0; i < glyphCount; ++i) {
        hb_codepoint_t glyphid = glyph_info[i].codepoint;
        double x_offset = glyph_pos[i].x_offset / 64.0;
        double y_offset = glyph_pos[i].y_offset / 64.0;
        double x_advance = glyph_pos[i].x_advance / 64.0;
        double y_advance = glyph_pos[i].y_advance / 64.0;
        // draw_glyph(glyphid, cursor_x + x_offset, cursor_y + y_offset);
        // load glyph image into the slot (erase previous one)
        int32_t error = FT_Load_Glyph(face, glyphid, FT_LOAD_NO_HINTING);
        if (error) {
            printf("Error loading font glyph %d\n - continuing", error);
            continue; // ignore errors
        }
        error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        if (error) {
            printf("Error rendering font glyph %d\n - continuing", error);
            continue; // ignore errors
        }

        // now, draw to our target surface
        double x = cursor_x + x_offset + face->glyph->bitmap_left;
        double y = cursor_y + y_offset - face->glyph->bitmap_top;
        DrawFreeTypeBitmap(&face->glyph->bitmap,
                           x,
                           y,
                           cmd->color,
                           framebuffer,
                           framebuffer_width,
                           framebuffer_height);
        cursor_x += x_advance;
        cursor_y += y_advance;
    }

    hb_buffer_destroy(buf);

    Profiler_End;
}

static bool RenderCommandsChanged()
{
    if (*g_renderCommands.currentIndex != *g_renderCommands.previousIndex) {
        return true;
    }

    int32_t numCommands = *g_renderCommands.currentIndex;
    for (int32_t i = 0; i < numCommands; i++) {
        RenderCommand *current = &g_renderCommands.current[i];
        RenderCommand *previous = &g_renderCommands.previous[i];
        if (!RenderCommandsEqual(current, previous)) {
            return true;
        }
    }

    return false;
}

bool Render_Init()
{
    g_fontCache.harfBuzzFeatures[0].tag = HB_TAG('k', 'e', 'r', 'n');
    g_fontCache.harfBuzzFeatures[0].value = 1;
    g_fontCache.harfBuzzFeatures[0].start = 0;
    g_fontCache.harfBuzzFeatures[0].end = (uint32_t)-1;
    g_fontCache.harfBuzzFeatures[1].tag = HB_TAG('l', 'i', 'g', 'a');
    g_fontCache.harfBuzzFeatures[1].value = 1;
    g_fontCache.harfBuzzFeatures[1].start = 0;
    g_fontCache.harfBuzzFeatures[1].end = (uint32_t)-1;
    g_fontCache.harfBuzzFeatures[2].tag = HB_TAG('c', 'l', 'i', 'g');
    g_fontCache.harfBuzzFeatures[2].value = 1;
    g_fontCache.harfBuzzFeatures[2].start = 0;
    g_fontCache.harfBuzzFeatures[2].end = (uint32_t)-1;

    int32_t error = FT_Init_FreeType(&g_fontCache.fontLibrary);
    if (error) {
        printf("Failed to init FreeType\n");
        return false;
    }

    for (int32_t i = 0; i < FONT_COUNT; i++) {
        const char *fontFile = GetFontFile((Font)i);
        FT_Face face;
        error = FT_New_Face(g_fontCache.fontLibrary, fontFile, 0, &face);
        if (error == FT_Err_Unknown_File_Format) {
            // TODO: Proper cleanup of faces and ft library
            puts("Unknown font format");
            return false;
        } else if (error) {
            printf("Unknown error %d\n", error);
            return false;
        }

        hb_font_t *hbFont = hb_ft_font_create_referenced(face);
        if (hbFont == NULL) {
            printf("Failed to create hb_font\n");
            return false;
        }
        hb_ft_font_set_load_flags(hbFont, FT_LOAD_NO_HINTING);

        g_fontCache.faceCache[i] = face;
        g_fontCache.hbFontCache[i] = hbFont;
    }

    g_renderCommands.current = g_renderCommands.commands1;
    g_renderCommands.previous = g_renderCommands.commands2;
    g_renderCommands.currentIndex = &g_renderCommands.commandsIndex1;
    g_renderCommands.previousIndex = &g_renderCommands.commandsIndex2;

    return true;
}

void Render_Destroy()
{
    for (int32_t i = 0; i < FONT_COUNT; i++) {
        hb_font_destroy(g_fontCache.hbFontCache[i]);
        FT_Done_Face(g_fontCache.faceCache[i]);
    }
    FT_Done_Library(g_fontCache.fontLibrary);
}

void Render_BeginFrame()
{

    // Swap the current render queue.
    RenderCommand *tmpCommands = g_renderCommands.current;
    g_renderCommands.current = g_renderCommands.previous;
    g_renderCommands.previous = tmpCommands;

    int32_t *tmpIndex = g_renderCommands.currentIndex;
    g_renderCommands.currentIndex = g_renderCommands.previousIndex;
    g_renderCommands.previousIndex = tmpIndex;

    // Reset the new current render queue
    *g_renderCommands.currentIndex = 0;
}

void Render_EndFrame(eva_pixel *framebuffer,
                     int32_t framebuffer_width,
                     int32_t framebuffer_height,
                     float scale_x,
                     float scale_y)
{
    Profiler_Begin;

    if (RenderCommandsChanged()) {
        // Process current queue.
        int32_t numCommands = *g_renderCommands.currentIndex;
        for (int32_t i = 0; i < numCommands; i++) {
            RenderCommand *cmd = &g_renderCommands.current[i];
            switch (cmd->type) {
            case RENDER_COMMAND_RECT:
                DrawRect(cmd->rect,
                         &cmd->rectCommand,
                         framebuffer,
                         framebuffer_width,
                         framebuffer_height,
                         scale_x,
                         scale_y);
                break;
            case RENDER_COMMAND_FONT:
                DrawFont(cmd->rect,
                         &cmd->fontCommand,
                         framebuffer,
                         framebuffer_width,
                         framebuffer_height,
                         scale_x,
                         scale_y);
                break;
            }
        }
    }

    Profiler_End;
}

static void AddRenderRectCommand(Rect r, Color c)
{
    int32_t index = (*g_renderCommands.currentIndex)++;
    RenderCommand *cmd = &g_renderCommands.current[index];
    cmd->type = RENDER_COMMAND_RECT;
    cmd->rect = r;
    cmd->rectCommand.color = c;
}

static void
AddRenderFontCommand(Font font, const char *text, int32_t x, int32_t y, int32_t ptSize, Color c)
{
    int32_t index = (*g_renderCommands.currentIndex)++;
    RenderCommand *cmd = &g_renderCommands.current[index];
    cmd->type = RENDER_COMMAND_FONT;
    cmd->fontCommand.font = font;
    cmd->fontCommand.text = text;
    cmd->fontCommand.x = x;
    cmd->fontCommand.y = y;
    cmd->fontCommand.ptSize = ptSize;
    cmd->fontCommand.color = c;
}

// TODO: Might be worth adding RENDER_CLEAR_COMMAND as a special
//       case since it is CPU heavy but easy to optimise.
// static void DrawClear(Color color)
//{
//    Profiler_Begin;
//
//    assert(g_renderContext.initialized);
//
//    Pixel coloredPixel = ColorToPixel(color);
//    int32_t size = g_renderContext.frameBuffer.height *
//    g_renderContext.frameBuffer.width; Pixel *pixels =
//    (Pixel*)g_renderContext.frameBuffer.pixels; for (int32_t y = 0; y < size;
//    y++)
//    {
//        *pixels = coloredPixel;
//        pixels++;
//    }
//
//    Profiler_End;
//}
//
void Render_Clear(Color color)
{
    Rect r = {
        .x = 0, .y = 0, .w = eva_get_framebuffer_width(), .h = eva_get_framebuffer_height()
    };
    AddRenderRectCommand(r, color);
}

void Render_DrawRect(Rect rect,
                     Color color) // TODO: These should be passed by pointer
{
    AddRenderRectCommand(rect, color);
}

// void Render_DrawHollowRect(
//    Rect    rect,
//    Color   color,
//    int32_t borderThickness) // TODO: These should be passed by pointer
//{
//    Profiler_Begin;
//
//    assert(g_renderContext.initialized);
//
//    ScaleRect(&rect);
//    DrawHollowRectUnscaled(rect, color, borderThickness);
//
//    Profiler_End;
//}

void Render_DrawFont(
    Font font, const char *text, int32_t posX, int32_t posY, int32_t ptSize, Color c)
{
    AddRenderFontCommand(font, text, posX, posY, ptSize, c);
}

int32_t Render_GetTextWidth(Font font, const char *text, int32_t ptSize)
{
    Profiler_Begin;

    hb_buffer_t *buf;
    buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text, -1, 0, -1);
    hb_buffer_guess_segment_properties(buf);

    FT_Face face;
    hb_font_t *hbFont;
    float scale_x = eva_get_framebuffer_scale_x();
    float scale_y = eva_get_framebuffer_scale_y();
    LoadCachedFont(font, &face, &hbFont, ptSize, scale_x, scale_y);

    hb_shape(hbFont, buf, g_fontCache.harfBuzzFeatures, HARFBUZZ_NUM_FEATURES);
    uint32_t glyphCount = 0;
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyphCount);

    double cursorX = 0;

    // TODO: Make a reusable buffer here.
    for (uint32_t i = 0; i < glyphCount; ++i) {
        double xAdvance = glyph_pos[i].x_advance / 64.0;

        cursorX += xAdvance;
    }

    hb_buffer_destroy(buf);

    Profiler_End;

    return (int32_t)(cursorX / scale_x);
}

void Render_GetFontHeight(Font font, int32_t ptSize, int32_t *ascent, int32_t *descent)
{
    Profiler_Begin;

    FT_Face face;
    hb_font_t *hbFont;
    float scale_x = eva_get_framebuffer_scale_x();
    float scale_y = eva_get_framebuffer_scale_y();
    LoadCachedFont(font, &face, &hbFont, ptSize, scale_x, scale_y);

    hb_font_extents_t extents;
    hb_font_get_h_extents(hbFont, &extents);

    *ascent = extents.ascender / 64;
    *descent = extents.descender / 64;

    Profiler_End;
}
