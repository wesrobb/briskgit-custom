#include "app.h"

#include <stdio.h>
#include <string.h>

#include "eva/eva.h"

#include "profiler.h"
#include "render.h"

#define TEXT_MAX_LEN 1000

typedef struct app_ctx {
    int32_t branch_pane_resize_range;
    eva_rect branch_pane_rect;
    bool branch_pane_resizing;
    int32_t branch_pane_min_size;

    char text[TEXT_MAX_LEN];
    size_t text_index;
} app_ctx;

static app_ctx _ctx;

static bool point_in_rect(int32_t x, int32_t y, eva_rect *r)
{
    return x >= r->x          &&
           x <= (r->x + r->w) &&
           y >= r->y          &&
           y <= (r->y + r->h);
}

void app_init()
{
    eva_framebuffer fb = eva_get_framebuffer();

    _ctx.branch_pane_resize_range = (int32_t)(5 * fb.scale_x);
    _ctx.branch_pane_rect.w = 400;
    _ctx.branch_pane_rect.h = (int32_t)fb.h;
    _ctx.branch_pane_min_size = 200;
}

void app_shutdown()
{
}

void app_keydown(const char* utf8_codepoint)
{
    size_t len = strlen(utf8_codepoint);
    if (_ctx.text_index + len < TEXT_MAX_LEN) {
        memcpy(&_ctx.text[_ctx.text_index], utf8_codepoint, len);
        _ctx.text_index += len;
    }
}

void app_mouse_moved(int32_t x, int32_t y)
{
    (void)y;
    if (_ctx.branch_pane_resizing) {
        _ctx.branch_pane_rect.w =
            max(x, _ctx.branch_pane_min_size);
        eva_request_frame();
    }

    eva_rect resizeHandle = {
        .x = _ctx.branch_pane_rect.x + _ctx.branch_pane_rect.w -
             _ctx.branch_pane_resize_range,
        .y = _ctx.branch_pane_rect.y,
        .w = _ctx.branch_pane_resize_range * 2,
        .h = _ctx.branch_pane_rect.y + _ctx.branch_pane_rect.h,
    };
    (void)resizeHandle;
}

void app_mouse_pressed(int32_t x, int32_t y)
{
    eva_rect resizeHandle = {
        .x = _ctx.branch_pane_rect.x + _ctx.branch_pane_rect.w -
            _ctx.branch_pane_resize_range,
        .y = _ctx.branch_pane_rect.y,
        .w = _ctx.branch_pane_resize_range * 2,
        .h = _ctx.branch_pane_rect.y + _ctx.branch_pane_rect.h,
    };

    if (point_in_rect(x, y, &resizeHandle)) {
        _ctx.branch_pane_resizing = true;
        puts("pane resizing");
    }
}

void app_mouse_released(int32_t x, int32_t y)
{
    (void)x;
    (void)y;
    if (_ctx.branch_pane_resizing) {
        _ctx.branch_pane_resizing = false;
        puts("pane stopped resizing");
    }
}

void app_window_resized(uint32_t width, uint32_t height)
{
    (void)width;
    (void)height;
    _ctx.branch_pane_rect.h = (int32_t)eva_get_window_height();
}

void app_draw()
{
    profiler_begin;

    int32_t font_size_pt = 14;

    color white = {
        .r = 1.0f,
        .g = 1.0f,
        .b = 1.0f,
        .a = 1.0f,
    };
    color light_grey = {
        .r = 0.1f,
        .g = 0.1f,
        .b = 0.1f,
        .a = 1.0f
    };
    color grey = {
        .r = 0.3f,
        .g = 0.3f,
        .b = 0.3f,
        .a = 1.0f
    };

    render_clear(light_grey);
    render_draw_rect(&_ctx.branch_pane_rect, grey);
    // Render_DrawHollowRect(testRect, white, 4);

    int32_t ascent, descent;
    render_get_font_height(FONT_ROBOTO_REGULAR, font_size_pt, 
                           &ascent, &descent);

    const char *text_lines[] = {
        "master",
        "develop",
        "feature/AV",
        "pppppppppp",
        "ffffffffff",
        _ctx.text
    };

    uint32_t num_text_lines = sizeof(text_lines) / sizeof(text_lines[0]);
    for (int32_t i = 0; i < (int32_t)num_text_lines; i++) {
        // int32_t width = Render_GetTextWidth(FONT_ROBOTO_REGULAR,
        // textLines[i], fontSizePt);
        int32_t x = 40;
        int32_t y = (int32_t)(40 + (i * (ascent - descent)));
        // Rect r = {
        //    .x = x,
        //    .y = y - ascent,
        //    .w = width,
        //    .h = ascent - descent
        //};

        render_draw_font(FONT_ROBOTO_REGULAR,
                         text_lines[i], 
                         x, y, font_size_pt, white);
    }

    profiler_end;
}
