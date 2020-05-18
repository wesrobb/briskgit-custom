#include "app.h"

#include <SDL.h>

#include "render.h"

bool App_Init()
{
    return Render_Init();
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

void App_Draw(SDL_Surface *surface, float dpiX, float dpiY)
{
    Rect rect = {
        .x = 100,
        .y = 100,
        .w = 200,
        .h = 200
    };
    Color clearColor = {
        .r = 0.1f,
        .g = 0.1f,
        .b = 0.1f,
        .a = 1.0f
    };
    Color color = {
        .r = 1.0f,
        .a = 1.0f
    };

    Render_Clear(surface, clearColor); 
    Render_DrawRect(surface, rect, color);  
    Render_DrawFont(surface, 100, 100, dpiX, dpiY, false);
    Render_DrawFont(surface, 100, 200, dpiX, dpiY, true);
    Render_DrawFontHarfBuzz(surface, 100, 300, dpiX, dpiY);
}
