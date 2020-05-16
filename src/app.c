#include "app.h"

#include <SDL.h>

#include "render.h"

void App_Init()
{
    Render_Init();
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

void App_OnWindowResized(int width, int height)
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
    Color color = {
        .r = 1.0f,
        .a = 1.0f
    };

    Render_DrawRect(surface, rect, color);  
    Render_DrawFont(surface, dpiX, dpiY);
}
