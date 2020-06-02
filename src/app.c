#include "app.h"

#include "eva/eva.h"

#include "profiler.h"
#include "render.h"

#include <stdio.h>

typedef struct AppContext {
    int32_t branchDrawerResizeRange;
    Rect branchDrawerRect;
    bool branchDrawerResizing;
    int32_t branchDrawerMinSize;

} AppContext;

AppContext g_appContext;

static bool point_in_rect(int32_t x, int32_t y, Rect *r)
{
    return x >= r->x && x <= (r->x + r->w) && y >= r->y && y <= (r->y + r->h);
}

void App_Init()
{
    float scale_x = eva_get_framebuffer_scale_x();

    g_appContext.branchDrawerResizeRange = (int32_t)(5 * scale_x);
    g_appContext.branchDrawerRect.w = 400;
    g_appContext.branchDrawerRect.h = eva_get_window_height();
    g_appContext.branchDrawerMinSize = 200;
}

void App_Destroy()
{
}

// void App_OnKeyPressed(SDL_Keysym *e) { (void)e; }
//
// void App_OnKeyReleased(SDL_Keysym *e) { (void)e; }

void App_OnMouseMoved(eva_mouse_event *e)
{
    if (g_appContext.branchDrawerResizing) {
        g_appContext.branchDrawerRect.w = max(e->x, g_appContext.branchDrawerMinSize);
        eva_request_frame();
    }

    Rect resizeHandle = {
        .x = g_appContext.branchDrawerRect.x + g_appContext.branchDrawerRect.w -
             g_appContext.branchDrawerResizeRange,
        .y = g_appContext.branchDrawerRect.y,
        .w = g_appContext.branchDrawerResizeRange * 2,
        .h = g_appContext.branchDrawerRect.y + g_appContext.branchDrawerRect.h,
    };
    (void)resizeHandle;

    // if (PointInRect(e->x, e->y, &resizeHandle)) {
    //  SDL_SetCursor(g_appContext.horizontalResizeCursor);
    //} else {
    //  SDL_SetCursor(g_appContext.defaultCursor);
    //}
}

void App_OnMousePressed(eva_mouse_event *e)
{
    (void)e;
    if (e->left_button_pressed) {
        Rect resizeHandle = {
            .x = g_appContext.branchDrawerRect.x + g_appContext.branchDrawerRect.w -
                 g_appContext.branchDrawerResizeRange,
            .y = g_appContext.branchDrawerRect.y,
            .w = g_appContext.branchDrawerResizeRange * 2,
            .h = g_appContext.branchDrawerRect.y + g_appContext.branchDrawerRect.h,
        };

        if (point_in_rect(e->x, e->y, &resizeHandle)) {
            g_appContext.branchDrawerResizing = true;
        }
    }
}

void App_OnMouseReleased(eva_mouse_event *e)
{
    (void)e;
    if (e->left_button_released) {
        if (g_appContext.branchDrawerResizing) {
            g_appContext.branchDrawerResizing = false;
        }
    }
}

void App_OnWindowResized(int32_t width, int32_t height)
{
    (void)width;
    (void)height;
    g_appContext.branchDrawerRect.h = eva_get_window_height();

    eva_request_frame();
}

void App_Draw()
{
    Profiler_Begin;

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
    // Render_DrawHollowRect(testRect, white, 4);

    int32_t fontSizePt = 18;
    // int32_t ascent, descent;
    // Render_GetFontHeight(FONT_ROBOTO_REGULAR, fontSizePt, &ascent, &descent);

    const char *textLines[] = {
        "master",
        "develop",
        "feature/AV",
        "pppppppppp",
        "ffffffffff"
    };

    for (int32_t i = 0; (unsigned long)i < sizeof(textLines) / sizeof(textLines[0]); i++) {
        // int32_t width = Render_GetTextWidth(FONT_ROBOTO_REGULAR,
        // textLines[i], fontSizePt);
        int32_t x = 10;
        int32_t y = 20 + (i * (22));
        // Rect r = {
        //    .x = x,
        //    .y = y - ascent,
        //    .w = width,
        //    .h = ascent - descent
        //};

        // Render_DrawHollowRect(r, white, 2);
        Render_DrawFont(FONT_ROBOTO_REGULAR, textLines[i], x, y, fontSizePt, white);
    }

    Profiler_End;
}
