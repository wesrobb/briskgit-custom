#pragma once

#include "common.h"

typedef struct Rect
{
    int32_t x, y, w, h;
} Rect;

typedef struct Color
{
    float b, g, r, a;
} Color;

typedef struct SDL_Surface SDL_Surface;
bool Render_Init();
void Render_Destroy();
void Render_Clear(SDL_Surface *, Color);
void Render_DrawRect(SDL_Surface *, Rect, Color);
void Render_DrawFont(SDL_Surface *, int32_t posX, int32_t posY, float dpiX, float dpiY, bool useKerning);
void Render_DrawFontHarfBuzz(SDL_Surface *, int32_t posX, int32_t posY, float dpiX, float dpiY);
