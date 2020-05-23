#include "app.h"

#include <SDL.h>

#include "SDL_events.h"
#include "SDL_mouse.h"
#include "render.h"

typedef struct AppContext
{
    int32_t branchDrawerResizeRange;
    Rect branchDrawerRect;
    bool branchDrawerResizing;

    SDL_Cursor *defaultCursor;
    SDL_Cursor *horizontalResizeCursor;
} AppContext;

AppContext g_appContext;

static bool PointInRect(int32_t x, int32_t y, Rect *r)
{
    return x >= r->x &&
           x <= (r->x + r->w) &&
           y >= r->y &&
           y <= (r->y + r->h);
}

void App_Init()
{
    int32_t frameWidth, frameHeight;
    float scaleFactorX, scaleFactorY;
    Render_GetDimensions(&frameWidth, &frameHeight, &scaleFactorX, &scaleFactorY);

    g_appContext.branchDrawerResizeRange = (int32_t)(5 * scaleFactorX);
    g_appContext.branchDrawerRect.w = 400;
    g_appContext.branchDrawerRect.h = frameHeight;

    g_appContext.defaultCursor = SDL_GetCursor(); // Store this switching back and forth between other cursors.
    g_appContext.horizontalResizeCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
}

void App_Destroy()
{
    SDL_FreeCursor(g_appContext.horizontalResizeCursor);
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
    if (g_appContext.branchDrawerResizing)
    {
        g_appContext.branchDrawerRect.w = e->x;
    }

    Rect resizeHandle = {
        .x = g_appContext.branchDrawerRect.x + g_appContext.branchDrawerRect.w - g_appContext.branchDrawerResizeRange,
        .y = g_appContext.branchDrawerRect.y,
        .w = g_appContext.branchDrawerResizeRange * 2,
        .h = g_appContext.branchDrawerRect.y + g_appContext.branchDrawerRect.h,
    };

    if (PointInRect(e->x, e->y, &resizeHandle))
    {
        SDL_SetCursor(g_appContext.horizontalResizeCursor);
    }
    else
    {
        SDL_SetCursor(g_appContext.defaultCursor);
    }
}

void App_OnMousePressed(SDL_MouseButtonEvent *e)
{
    if (e->button & SDL_BUTTON_LEFT)
    {
        printf("Left mouse button pressed\n");
        Rect resizeHandle = {
            .x = g_appContext.branchDrawerRect.x + g_appContext.branchDrawerRect.w - g_appContext.branchDrawerResizeRange,
            .y = g_appContext.branchDrawerRect.y,
            .w = g_appContext.branchDrawerResizeRange * 2,
            .h = g_appContext.branchDrawerRect.y + g_appContext.branchDrawerRect.h,
        };

        if (PointInRect(e->x, e->y, &resizeHandle))
        {
            g_appContext.branchDrawerResizing = true;
        }
    }
}

void App_OnMouseReleased(SDL_MouseButtonEvent *e)
{
    if (e->button & SDL_BUTTON_LEFT)
    {
        printf("Left mouse button released\n");
        if (g_appContext.branchDrawerResizing)
        {
            g_appContext.branchDrawerResizing = false;
        }
    }
}

void App_OnWindowResized(int32_t width, int32_t height)
{
    (void)width;
    (void)height;
}

void App_Draw()
{
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
    Render_DrawRect(g_appContext.branchDrawerRect, color);
    Render_DrawFont("master", 10, 20, 14, white);
    Render_DrawFont("develop", 10, 50, 14, white);
    Render_DrawFont("feature/AV", 10, 80, 14, white);
}
