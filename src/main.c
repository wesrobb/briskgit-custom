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

static void event(eva_event *e)
{
    switch (e->type) {
    case EVA_EVENTTYPE_WINDOW:
        app_window_resized(e->window.window_width, e->window.window_height);
        break;
    case EVA_EVENTTYPE_KB:
        puts("Received eva kb event");
        handle_kb_event(&e->kb);
        break;
    case EVA_EVENTTYPE_QUITREQUESTED:
        puts("Received eva quit requested");
        return;
    case EVA_EVENTTYPE_REDRAWFRAME:
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

static void mouse_moved(int32_t x, int32_t y)
{
    app_mouse_moved(x, y);

    render_begin_frame();
    app_draw();
    render_end_frame();
    eva_request_frame();
}

static void mouse_btn(int32_t x, int32_t y,
                      eva_mouse_btn btn, eva_mouse_action action)
{
    if (btn == EVA_MOUSE_BTN_LEFT) {
        if (action == EVA_MOUSE_PRESSED) {
            app_mouse_pressed(x, y);
        }
        if (action == EVA_MOUSE_RELEASED) {
            app_mouse_released(x, y);
        }
    }

    render_begin_frame();
    app_draw();
    render_end_frame();
    eva_request_frame();
}

int main()
{
    printf("Hello briskgit!\n");

    eva_set_mouse_moved_fn(mouse_moved);
    eva_set_mouse_btn_fn(mouse_btn);
    eva_run("Briskgit", init, event, cleanup, fail);

    return 0;
}
