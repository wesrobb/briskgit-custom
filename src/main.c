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

static void handle_key(eva_key key, eva_input_action action,
                       eva_mod_flags mods)
{
    (void)action;
    (void)mods;
    if (action == EVA_INPUT_PRESSED) {
        app_keydown(key);
    }
}

static void handle_text(const char *utf8_text, uint32_t len, eva_mod_flags mods)
{
    (void)mods;

    app_text_input(utf8_text, len);
}

static void event(eva_event *e)
{
    switch (e->type) {
    case EVA_EVENTTYPE_WINDOW:
        app_window_resized(e->window.window_width, e->window.window_height);
        break;
    case EVA_EVENTTYPE_REDRAWFRAME:
        render_begin_frame();
        app_draw();
        render_end_frame();
        eva_request_frame();
        break;
    }
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
}

static void mouse_btn(int32_t x, int32_t y,
                      eva_mouse_btn btn, eva_input_action action)
{
    if (btn == EVA_MOUSE_BTN_LEFT) {
        if (action == EVA_INPUT_PRESSED) {
            app_mouse_pressed(x, y);
        }
        if (action == EVA_INPUT_RELEASED) {
            app_mouse_released(x, y);
        }
    }

    eva_request_frame();
}

static void frame(const eva_framebuffer *fb)
{
    (void)fb;

    render_begin_frame();
    app_draw();
    render_end_frame();

    profiler_log(0);
}

static bool cancel_quit(void)
{
    return false;
}

int main()
{
    printf("Hello briskgit!\n");

    eva_set_mouse_moved_fn(mouse_moved);
    eva_set_mouse_btn_fn(mouse_btn);
    eva_set_key_fn(handle_key);
    eva_set_text_input_fn(handle_text);
    eva_set_cancel_quit_fn(cancel_quit);
    eva_set_init_fn(init);
    eva_set_cleanup_fn(cleanup);
    eva_run("Briskgit", event, frame, fail);

    return 0;
}
