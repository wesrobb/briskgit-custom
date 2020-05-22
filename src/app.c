#include "app.h"

#include <SDL.h>

#include "render.h"

void App_Init()
{
}

void App_Destroy()
{
    Render_Destroy();
}

void App_OnKeyPressed(SDL_Keysym *e)
{
    (void)e;
}

void App_OnKeyReleased(SDL_Keysym *e)
{
    (void)e;
}

void App_OnMouseMoved(SDL_MouseMotionEvent *e)
{
    (void)e;
}

void App_OnMousePressed(SDL_MouseEvent *e)
{
    (void)e;
}

void App_OnMouseReleased(SDL_MouseEvent *e)
{
    (void)e;
}

void App_OnWindowResized(int32_t width, int32_t height)
{
    (void)width;
    (void)height;
}

void App_Draw()
{
    int32_t frameWidth, frameHeight;
    Render_GetDimensions(&frameWidth, &frameHeight);

    Rect rect = {
        .x = 0,
        .y = 0,
        .w = 400,
        .h = frameHeight
    };
    Color clearColor = {
        .r = 0.1f,
        .g = 0.1f,
        .b = 0.1f,
        .a = 1.0f
    };
    Color color = {
        .r = 0.3f,
        .g = 0.3f,
        .b = 0.3f,
        .a = 1.0f
    };

    Render_Clear(clearColor); 
    Render_DrawRect(rect, color);  
    Render_DrawFont(100, 100, false);
    Render_DrawFont(100, 200, true);
    Render_DrawFontHarfBuzz(100, 300, false);
    Render_DrawFontHarfBuzz(100, 400, true);
}
