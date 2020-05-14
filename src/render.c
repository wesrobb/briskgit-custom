#include "render.h"

#include <SDL.h>

#include "SDL_video.h"
#include "common.h"

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
