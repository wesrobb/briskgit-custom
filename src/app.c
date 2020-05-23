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

    Color white = {
        .r = 1.0f,
        .g = 1.0f,
        .b = 1.0f,
        .a = 1.0f,
    };

    Render_Clear(clearColor); 
    Render_DrawRect(rect, color);  
    Render_DrawFont("master", 10, 20, 14, white);
    Render_DrawFont("develop", 10, 50, 14, white);
    Render_DrawFont("feature/AV", 10, 80, 14, white);
}
