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

static void handle_kb_event(eva_kb_event *e)
{
    switch (e->type) {
        case EVA_KB_EVENTTYPE_KEYDOWN:
            app_keydown(e->utf8_codepoint);
            break;
    }
}

static void handle_mouse_event(eva_mouse_event *e)
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

static void event(eva_event *e)
{
    bool full_redraw = false;

    switch (e->type) {
    case EVA_EVENTTYPE_WINDOW:
        app_window_resized(e->window.window_width, e->window.window_height);
        break;
    case EVA_EVENTTYPE_MOUSE:
        handle_mouse_event(&e->mouse);
        break;
    case EVA_EVENTTYPE_KB:
        puts("Received eva kb event");
        handle_kb_event(&e->kb);
        break;
    case EVA_EVENTTYPE_QUITREQUESTED:
        puts("Received eva quit requested");
        return;
    case EVA_EVENTTYPE_REDRAWFRAME:
        full_redraw = true;
        break;
    }

    render_begin_frame();
    app_draw();
    render_end_frame();

    eva_request_frame();

    profiler_log(2);
}

static void cleanup(void)
{
    puts("Cleaning up");
    app_shutdown();
    render_shutdown();
}

static void fail(int error_code, const char *error_message)
{
    printf("Error %d: %s\n", error_code, error_message);
}

int main()
{
    printf("Hello briskgit!\n");

    eva_run("Briskgit", init, event, cleanup, fail);

    return 0;
}
