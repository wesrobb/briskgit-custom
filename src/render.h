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

typedef enum Font
{
    FONT_ROBOTO_REGULAR,
    FONT_MENLO_REGULAR,
    FONT_COUNT
} Font;

bool Render_Init(int32_t width, int32_t height, float scaleFactorX, float scaleFactorY);
void Render_Destroy();
bool Render_Update(int32_t width, int32_t height, float scaleFactorX, float scaleFactorY);

void Render_GetDimensions(int32_t *width, int32_t *height, float *scaleFactorX, float *scaleFactorY);

void Render_BeginFrame();
FrameBuffer *Render_EndFrame();

void Render_Clear(Color);
void Render_DrawRect(Rect, Color);
void Render_DrawHollowRect(Rect rect, Color color, int32_t borderThickness);
void Render_DrawFont(Font font, const char *text, int32_t posX, int32_t posY, int32_t ptSize, Color color);
int32_t Render_GetTextWidth(Font font, const char* text, int32_t ptSize);
void Render_GetFontHeight(Font font, int32_t ptSize, int32_t *ascent, int32_t *descent);
