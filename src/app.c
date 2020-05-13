#include "app.h"

#include <SDL.h>

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
    printf("Mouse moved %d,%d\n", e->x, e->y);
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

void App_Draw(SDL_Surface *s)
{
    (void)s;
}
