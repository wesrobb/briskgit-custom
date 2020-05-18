#include "render.h"

#include <math.h>
#include <string.h>

#include <SDL.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include <hb.h>
#include <hb-ft.h>

#include "SDL_video.h"
#include "common.h"

typedef struct Pixel
{
    uint8_t b, g, r, a;
} Pixel;

typedef struct RenderContext {
    FT_Library fontLibrary;
    FT_Face currentFontFace;

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

static void DrawFreeTypeBitmap(SDL_Surface *surface, FT_Bitmap *bitmap, int32_t x, int32_t y)
{
    Color fontColorLinear = {
        .g = 1.0f,
        .b = 1.0f
    }; // Assumes linear color space not sRGB

    SDL_assert(surface);
    SDL_assert(bitmap);

    int32_t startX = max(surface->clip_rect.x, x);
    int32_t endX = min(surface->clip_rect.x + surface->clip_rect.w, x + (int32_t)bitmap->width);
    int32_t startY = max(surface->clip_rect.y, y);
    int32_t endY = min(surface->clip_rect.y + surface->clip_rect.h, y + (int32_t)bitmap->rows);

    Pixel *destPixels = (Pixel*)surface->pixels;
    destPixels += startX + startY * surface->w;  // Move to the first pixel that resides in the rect
    int32_t nextRow = surface->w - (endX - startX); // Calculate how many pixels to jump over at the end of each row in the rect.

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

bool Render_Init()
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

    return true;
}

void Render_Destroy()
{
    FT_Done_Face(g_renderContext.currentFontFace);
    FT_Done_Library(g_renderContext.fontLibrary);
}

void Render_Clear(SDL_Surface *surface, Color color)
{
    SDL_assert(surface);
    SDL_assert(g_renderContext.initialized);

    Pixel *pixels = (Pixel*)surface->pixels;
    Pixel coloredPixel = ColorToPixel(color);
    for (int32_t y = 0; y < surface->h; y++)
    {
        for (int32_t x = 0; x < surface->w; x++)
        {
            *pixels = coloredPixel;
            pixels++;
        }
    }
}

void Render_DrawRect(SDL_Surface *surface, Rect rect, Color color)
{
    SDL_assert(surface);
    SDL_assert(g_renderContext.initialized);

    int32_t startX = max(surface->clip_rect.x, rect.x);
    int32_t endX = min(surface->clip_rect.x + surface->clip_rect.w, rect.x + rect.w);
    int32_t startY = max(surface->clip_rect.y, rect.y);
    int32_t endY = min(surface->clip_rect.y + surface->clip_rect.h, rect.y + rect.h);

    Pixel *pixels = (Pixel*)surface->pixels;
    pixels += startX + startY * surface->w;  // Move to the first pixel that resides in the rect
    int32_t nextRow = surface->w - (endX - startX); // Calculate how many pixels to jump over at the end of each row in the rect.

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

void Render_DrawFont(SDL_Surface *surface, int32_t posX, int32_t posY, float dpiX, float dpiY, bool useKerning)
{
    SDL_assert(g_renderContext.initialized);

    FT_Face face = g_renderContext.currentFontFace;

    int32_t error = FT_Set_Char_Size(
          face,    /* handle to face object           */
          0,       /* char_width in 1/64th of points  */
          12*64,   /* char_height in 1/64th of points */
          (uint32_t)dpiX,    /* horizontal dpi                  */
          (uint32_t)dpiY);   /* vertical dpi                    */
    if (error)
    {
        puts("Failed to set face size");
        return;
    }

    FT_GlyphSlot  slot = face->glyph;  /* a small shortcut */

    useKerning = FT_HAS_KERNING(face) && useKerning;

    const char* text = "Hello AV.";
    size_t num_chars = strlen(text);
    FT_UInt previous = 0;

    for (size_t n = 0; n < num_chars; n++)
    {
        /* convert character code to glyph index */
        FT_UInt glyphIndex = FT_Get_Char_Index(face, (FT_ULong)text[n]);

        /* retrieve kerning distance and move pen position */
        if (useKerning && previous && glyphIndex)
        {
            FT_Vector delta;
            FT_Get_Kerning(face, previous, glyphIndex, FT_KERNING_DEFAULT, &delta);
            posX += delta.x >> 6;
        }

        /* load glyph image into the slot (erase previous one) */
        error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER);
        if (error)
        {
            printf("Error loading font glyph %d\n - continuing", error);
            continue;  /* ignore errors */
        }

        /* now, draw to our target surface */
        DrawFreeTypeBitmap(surface, &slot->bitmap,
                           posX + slot->bitmap_left,
                           posY - slot->bitmap_top);

        /* increment pen position */
        posX += slot->advance.x >> 6;
        previous = glyphIndex;
    }
}

void Render_DrawFontHarfBuzz(SDL_Surface *surface, int32_t posX, int32_t posY, float dpiX, float dpiY)
{
    const char *text = "Hello AV.";
    hb_buffer_t *buf;
    buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text, -1, 0, -1);

    hb_buffer_guess_segment_properties(buf);

    FT_Face face = g_renderContext.currentFontFace;

    int32_t error = FT_Set_Char_Size(
          face,    /* handle to face object           */
          0,       /* char_width in 1/64th of points  */
          12*64,   /* char_height in 1/64th of points */
          (uint32_t)dpiX,    /* horizontal dpi                  */
          (uint32_t)dpiY);   /* vertical dpi                    */
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
        /* load glyph image into the slot (erase previous one) */
          error = FT_Load_Glyph(face, glyphid, FT_LOAD_RENDER);
          if (error)
          {
              printf("Error loading font glyph %d\n - continuing", error);
              continue;  /* ignore errors */
          }

          /* now, draw to our target surface */
          DrawFreeTypeBitmap(surface, &face->glyph->bitmap,
                  (int32_t)(cursor_x + x_offset),
                  (int32_t)(cursor_y + y_offset - face->glyph->bitmap_top));
          cursor_x += x_advance;
          cursor_y += y_advance;
      }
}
