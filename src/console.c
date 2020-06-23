#include "console.h"

#include "eva/eva.h"

#include "render.h"

typedef struct console_ctx {
    bool visible;
    eva_rect rect;
} console_ctx;

static console_ctx _ctx;

void console_keydown(int32_t key, uint32_t mods)
{
    eva_key k = (eva_key)key;
    eva_mod_flags f = (eva_mod_flags)mods;

    // Ctrl+` opens/closes the console
    if (k == EVA_KEY_GRAVE_ACCENT && f == EVA_MOD_CONTROL) {
        _ctx.visible = !_ctx.visible;
        eva_request_frame();
    }
}

void console_draw(const eva_framebuffer *fb)
{
    if (_ctx.visible) {
        color base_color = {
            .r = 0.5f,
            .g = 0.5f,
            .b = 0.5f,
            .a = 1.0f,
        };

        eva_rect rect = {
            .x = 0,
            .y = 0,
            .w = (int32_t)fb->w,
            .h = (int32_t)(fb->h / 3.0f)
        };
        render_draw_rect(&rect, base_color);
    }
}
