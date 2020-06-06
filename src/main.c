#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "eva/eva.h"

#include "app.h"
#include "profiler.h"
#include "render.h"

void init(void)
{
    profiler_init;

    render_init();
    app_init();
}

void handle_mouse_event(eva_mouse_event *e)
{
    switch (e->type) {
    case EVA_MOUSE_EVENTTYPE_MOUSE_MOVED:
        app_mouse_moved(e);
        break;
    case EVA_MOUSE_EVENTTYPE_MOUSE_PRESSED:
        app_mouse_pressed(e);
        break;
    case EVA_MOUSE_EVENTTYPE_MOUSE_RELEASED:
        app_mouse_released(e);
        break;
    case EVA_MOUSE_EVENTTYPE_MOUSE_ENTERED:
        puts("Mouse entered");
        break;
    case EVA_MOUSE_EVENTTYPE_MOUSE_EXITED:
        puts("Mouse exited");
        break;
    }
}

void event(eva_event *e)
{
    bool full_redraw = false;

    switch (e->type) {
    case EVA_EVENTTYPE_WINDOW:
        puts("Received eva window event");
        app_window_resized(e->window.window_width, e->window.window_height);
        break;
    case EVA_EVENTTYPE_MOUSE:
        handle_mouse_event(&e->mouse);
        break;
    case EVA_EVENTTYPE_KEYBOARD:
        puts("Received eva keyboard event");
        break;
    case EVA_EVENTTYPE_QUITREQUESTED:
        puts("Received eva quit requested");
        return;
    case EVA_EVENTTYPE_REDRAWFRAME:
        full_redraw = true;
        break;
    }

    render_begin_frame();
    {
        profiler_begin_name("App_Draw");
        app_draw();
        profiler_end;
    }

    eva_rect dirty_rect;
    render_end_frame(&dirty_rect);

    if (full_redraw) {
        eva_request_frame(NULL);
    }
    else {
        eva_request_frame(&dirty_rect);
    }

    profiler_log(2);
}

void cleanup(void)
{
    puts("Cleaning up");
    app_shutdown();
    render_shutdown();
}

void fail(int error_code, const char *error_message)
{
    printf("Error %d: %s\n", error_code, error_message);
}

int main()
{
    printf("Hello briskgit!\n");

    eva_run("Briskgit", init, event, cleanup, fail);

    return 0;
}
