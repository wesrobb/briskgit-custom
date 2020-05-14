#include "render.h"

#include "freetype/ftimage.h"
#include "freetype/ftmodapi.h"
#include "string.h"

#include <SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "SDL_video.h"
#include "common.h"

void DrawFreeTypeBitmap(SDL_Window *window, FT_Bitmap *bitmap, int x, int y)
{
    SDL_assert(window);
    SDL_assert(bitmap);

    SDL_Surface *s = SDL_GetWindowSurface(window);

    int startX = max(s->clip_rect.x, x);
    int endX = min(s->clip_rect.x + s->clip_rect.w, x + (int)bitmap->width);
    int startY = max(s->clip_rect.y, y);
    int endY = min(s->clip_rect.y + s->clip_rect.h, y + (int)bitmap->rows);

    Pixel *pixels = (Pixel*)s->pixels;
    pixels += startX + startY * s->w;  // Move to the first pixel that resides in the rect
    int nextRow = s->w - (endX - startX); // Calculate how many pixels to jump over at the end of each row in the rect.

    for (int y = startY, v = 0; y < endY; y++, v++)
    {
        for (int x = startX, u = 0; x < endX; x++, u++)
        {
            unsigned char fontPx = bitmap->buffer[v * bitmap->width + u];
            Pixel px = {
                .g = fontPx,
                .b = fontPx,
                .a = 255
            };
            *pixels = px;
            pixels++;
        }

        pixels += nextRow;
    }
}

void Render_DrawRect(SDL_Window *window, Rect rect, Color color)
{
    SDL_assert(window);

    SDL_Surface *s = SDL_GetWindowSurface(window);

    int startX = max(s->clip_rect.x, rect.x);
    int endX = min(s->clip_rect.x + s->clip_rect.w, rect.x + rect.w);
    int startY = max(s->clip_rect.y, rect.y);
    int endY = min(s->clip_rect.y + s->clip_rect.h, rect.y + rect.h);

    Pixel *pixels = (Pixel*)s->pixels;
    pixels += startX + startY * s->w;  // Move to the first pixel that resides in the rect
    int nextRow = s->w - (endX - startX); // Calculate how many pixels to jump over at the end of each row in the rect.

    for (int y = startY; y < endY; y++)
    {
        for (int x = startX; x < endX; x++)
        {
            *pixels = color;
            pixels++;
        }

        pixels += nextRow;
    }
}

void Render_DrawFont(SDL_Window *window)
{
    (void)window;

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
          128,     /* horizontal device resolution    */
          128);    /* vertical device resolution      */
    if (error)
    {
        puts("Failed to set face size");
        return;
    }

    FT_GlyphSlot  slot = face->glyph;  /* a small shortcut */
    int           pen_x, pen_y, n;


    pen_x = 300;
    pen_y = 200;

    const char* text = "Hello, world!";
    int num_chars = strlen(text);

    for ( n = 0; n < num_chars; n++ )
    {
        /* load glyph image into the slot (erase previous one) */
        error = FT_Load_Char(face, text[n], FT_LOAD_RENDER);
        if ( error )
            continue;  /* ignore errors */

        /* now, draw to our target surface */
        DrawFreeTypeBitmap(window, &slot->bitmap,
                           pen_x + slot->bitmap_left,
                           pen_y - slot->bitmap_top);

        /* increment pen position */
        pen_x += slot->advance.x >> 6;
    }

    FT_Done_Face(face);
    FT_Done_Library(library);
}
