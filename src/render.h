#pragma once

typedef struct Rect
{
    int x, y, w, h;
} Rect;

typedef struct Color
{
    unsigned char b, g, r, a;
} Color;
typedef Color Pixel;

typedef struct SDL_Window SDL_Window;
void Render_DrawRect(SDL_Window *, Rect, Color);
