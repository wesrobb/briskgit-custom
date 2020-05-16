#include "render.h"

#include <math.h>
#include <string.h>

#include "freetype/ftimage.h"
#include "freetype/ftmodapi.h"

#include <SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "SDL_video.h"
#include "common.h"

typedef struct Pixel
{
    unsigned char b, g, r, a;
} Pixel;

typedef struct RenderContext {
    bool initialized;
} RenderContext;

static RenderContext g_renderContext;

static Pixel ColorToPixel(Color c)
{
    Pixel result = {
        .b = (unsigned char)(c.b * 255.999f),
        .g = (unsigned char)(c.g * 255.999f),
        .r = (unsigned char)(c.r * 255.999f),
        .a = (unsigned char)(c.a * 255.999f),
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
        .r = src.r - dest.r * (1.0f - src.a),
        .g = src.g - dest.g * (1.0f - src.a),
        .b = src.b - dest.b * (1.0f - src.a)
    };

    return result;
}

static void DrawFreeTypeBitmap(SDL_Surface *surface, FT_Bitmap *bitmap, int x, int y)
{
    Color fontColorLinear = {
        .g = 1.0f,
        .b = 1.0f
    }; // Assumes linear color space not sRGB

    SDL_assert(surface);
    SDL_assert(bitmap);

    int startX = max(surface->clip_rect.x, x);
    int endX = min(surface->clip_rect.x + surface->clip_rect.w, x + (int)bitmap->width);
    int startY = max(surface->clip_rect.y, y);
    int endY = min(surface->clip_rect.y + surface->clip_rect.h, y + (int)bitmap->rows);

    Pixel *destPixels = (Pixel*)surface->pixels;
    destPixels += startX + startY * surface->w;  // Move to the first pixel that resides in the rect
    int nextRow = surface->w - (endX - startX); // Calculate how many pixels to jump over at the end of each row in the rect.

    for (int y = startY, v = 0; y < endY; y++, v++)
    {
        for (int x = startX, u = 0; x < endX; x++, u++)
        {
            float fontAlphaLinear = bitmap->buffer[v * (int)bitmap->width + u] / 255.0f;
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

void Render_Init()
{
    g_renderContext.initialized = true;
}

void Render_DrawRect(SDL_Surface *surface, Rect rect, Color color)
{
    SDL_assert(surface);
    SDL_assert(g_renderContext.initialized);

    int startX = max(surface->clip_rect.x, rect.x);
    int endX = min(surface->clip_rect.x + surface->clip_rect.w, rect.x + rect.w);
    int startY = max(surface->clip_rect.y, rect.y);
    int endY = min(surface->clip_rect.y + surface->clip_rect.h, rect.y + rect.h);

    Pixel *pixels = (Pixel*)surface->pixels;
    pixels += startX + startY * surface->w;  // Move to the first pixel that resides in the rect
    int nextRow = surface->w - (endX - startX); // Calculate how many pixels to jump over at the end of each row in the rect.

    for (int y = startY; y < endY; y++)
    {
        for (int x = startX; x < endX; x++)
        {
            //*pixels = ColorToPixel(color);
            (void)color;
            Pixel p = {
                .r = 255,
                .a = 255
            };
            *pixels = p;
            pixels++;
        }

        pixels += nextRow;
    }
}

void Render_DrawFont(SDL_Surface *surface, float dpiX, float dpiY)
{
    SDL_assert(g_renderContext.initialized);

    FT_Library library;

    int error = FT_Init_FreeType(&library);
    if (error)
    {
        printf("Failed to init FreeType\n");
        return;
    }

    FT_Face face;
    error = FT_New_Face( library,
                     "data/Mukta-Regular.ttf",
                     0,
                     &face );
    if (error == FT_Err_Unknown_File_Format)
    {
        puts("Unknown font format");
        return;
    }
    else if (error)
    {
        printf("Unknown error %d\n", error);
        return;
    }

    error = FT_Set_Char_Size(
          face,    /* handle to face object           */
          0,       /* char_width in 1/64th of points  */
          12*64,   /* char_height in 1/64th of points */
          (unsigned int)dpiX,    /* horizontal dpi                  */
          (unsigned int)dpiY);   /* vertical dpi                    */
    if (error)
    {
        puts("Failed to set face size");
        return;
    }

    FT_GlyphSlot  slot = face->glyph;  /* a small shortcut */
    int           pen_x, pen_y;


    pen_x = 300;
    pen_y = 200;

    const char* text = "Hello, world!";
    size_t num_chars = strlen(text);

    for (size_t n = 0; n < num_chars; n++)
    {
        /* load glyph image into the slot (erase previous one) */
        error = FT_Load_Char(face, (FT_ULong)text[n], FT_LOAD_RENDER);
        if ( error )
            continue;  /* ignore errors */

        /* now, draw to our target surface */
        DrawFreeTypeBitmap(surface, &slot->bitmap,
                           pen_x + slot->bitmap_left,
                           pen_y - slot->bitmap_top);

        /* increment pen position */
        pen_x += slot->advance.x >> 6;
    }

    FT_Done_Face(face);
    FT_Done_Library(library);
}
