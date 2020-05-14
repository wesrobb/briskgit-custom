#include "app.h"

#include <SDL.h>

#include "render.h"

typedef struct AppState {
    int version;
} AppState;

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

void App_Draw(SDL_Window *window)
{
    Rect rect = {
        .x = 100,
        .y = 100,
        .w = 200,
        .h = 200
    };
    Color color = {
        .r = 255,
        .g = 0,
        .b = 0,
        .a = 255
    };

    Render_DrawRect(window, rect, color);  
}
