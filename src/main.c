#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "eva/eva.h"

#include "app.h"
#include "profiler.h"
#include "render.h"

void init()
{
    profiler_init;

    Render_Init();
    App_Init();
}

void frame(eva_pixel *framebuffer,
           int32_t framebuffer_width,
           int32_t framebuffer_height,
           float scale_x,
           float scale_y,
           eva_rect *dirty_rect)
{
    Render_BeginFrame();
    {
        Profiler_BeginName("App_Draw");
        App_Draw();
        Profiler_End;
    }
    Render_EndFrame(framebuffer, framebuffer_width, framebuffer_height, scale_x, scale_y);

    Profiler_Log(2);

    // TEST
    dirty_rect->x = 0;
    dirty_rect->y = 0;
    dirty_rect->w = framebuffer_width / 2;
    dirty_rect->h = framebuffer_height;
}

void handle_mouse_event(eva_mouse_event *e)
{
    switch (e->type) {
    case EVA_MOUSE_EVENTTYPE_MOUSE_MOVED:
        App_OnMouseMoved(e);
        break;
    case EVA_MOUSE_EVENTTYPE_MOUSE_PRESSED:
        App_OnMousePressed(e);
        break;
    case EVA_MOUSE_EVENTTYPE_MOUSE_RELEASED:
        App_OnMouseReleased(e);
        break;
    }
}

void event(eva_event *e)
{
    switch (e->type) {
    case EVA_EVENTTYPE_WINDOW:
        puts("Received eva window event");
        App_OnWindowResized(e->window.window_width, e->window.window_height);
        break;
    case EVA_EVENTTYPE_MOUSE:
        handle_mouse_event(&e->mouse);
        break;
    case EVA_EVENTTYPE_KEYBOARD:
        puts("Received eva keyboard event");
        break;
    case EVA_EVENTTYPE_QUITREQUESTED:
        puts("Received eva quit requested");
        break;
    }
}

void cleanup()
{
    puts("Cleaning up");
    App_Destroy();
    Render_Destroy();
}

void fail(int error_code, const char *error_message)
{
    printf("Error %d: %s\n", error_code, error_message);
}

int main()
{
    printf("Hello briskgit!\n");

    eva_run("Briskgit", init, frame, event, cleanup, fail);

    return 0;
}
