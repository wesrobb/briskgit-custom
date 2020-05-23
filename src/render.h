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

typedef struct Pixel
{
    uint8_t b, g, r, a;
} Pixel;

typedef struct FrameBuffer
{
    Pixel *pixels;
    int32_t width, height;
} FrameBuffer;

bool Render_Init(int32_t width, int32_t height, float dpiX, float dpiY, float scaleFactorX, float scaleFactorY);
void Render_Destroy();
bool Render_Update(int32_t width, int32_t height, float dpiX, float dpiY, float scaleFactorX, float scaleFactorY);

void Render_GetDimensions(int32_t *width, int32_t *height);
FrameBuffer *Render_GetFrameBuffer();

void Render_Clear(Color);
void Render_DrawRect(Rect, Color);
void Render_DrawFont(const char *text, int32_t posX, int32_t posY, int32_t ptSize, Color c);
