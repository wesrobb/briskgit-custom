#include "app.h"

#include <stdio.h>
#include <string.h>

#include "eva/eva.h"

#include "color.h"
#include "console.h"
#include "profiler.h"
#include "rect.h"
#include "render.h"
#include "text.h"
#include "ustr.h"
#include "vec2.h"

#define TEXT_MAX_LEN 1000

typedef struct app_ctx {
    int32_t branch_pane_resize_range;
    recti branch_pane_rect;
    bool branch_pane_resizing;
    int32_t branch_pane_min_size;

    char text[TEXT_MAX_LEN];
    size_t text_index;
} app_ctx;

static app_ctx _ctx;

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

void app_keydown(int32_t key, uint32_t mods)
{
    if (key == EVA_KEY_BACKSPACE) {
        _ctx.text[--_ctx.text_index] = 0;
        eva_request_frame();
    }

    if (key == EVA_KEY_ENTER && _ctx.text_index > 0) {
        console_logn(_ctx.text, _ctx.text_index);
        _ctx.text_index = 0;
        eva_request_frame();
    }

    console_keydown(key, mods);
}

void app_text_input(const char *text, uint32_t len)
{
    (void)text;
    (void)len;
    //if (_ctx.text_index + len < TEXT_MAX_LEN) {
    //    memcpy(_ctx.text + _ctx.text_index, text, len);
    //    _ctx.text_index += len;
    //    ustr str;
    //    str.data = (char*)text;
    //    str.len = _ctx.text_index;
    //    int32_t count = ustr_num_graphemes(&str);
    //    console_log("num graphemes %i", count);
    //    eva_request_frame();
    //}
}

void app_mouse_moved(const vec2i *mouse_pos)
{
    if (_ctx.branch_pane_resizing) {
        _ctx.branch_pane_rect.w = max(mouse_pos->x, _ctx.branch_pane_min_size);
        eva_request_frame();
    }

    recti resizeHandle = {
        .x = _ctx.branch_pane_rect.x + _ctx.branch_pane_rect.w -
             _ctx.branch_pane_resize_range,
        .y = _ctx.branch_pane_rect.y,
        .w = _ctx.branch_pane_resize_range * 2,
        .h = _ctx.branch_pane_rect.y + _ctx.branch_pane_rect.h,
    };
    (void)resizeHandle;

    console_mouse_moved(mouse_pos);
}

void app_mouse_pressed(const vec2i *mouse_pos)
{
    recti resizeHandle = {
        .x = _ctx.branch_pane_rect.x + _ctx.branch_pane_rect.w -
            _ctx.branch_pane_resize_range,
        .y = _ctx.branch_pane_rect.y,
        .w = _ctx.branch_pane_resize_range * 2,
        .h = _ctx.branch_pane_rect.y + _ctx.branch_pane_rect.h,
    };

    if (recti_point_intersect(&resizeHandle, mouse_pos)) {
        _ctx.branch_pane_resizing = true;
    }

    console_mouse_pressed(mouse_pos);
}

void app_mouse_released(const vec2i *mouse_pos)
{
    if (_ctx.branch_pane_resizing) {
        _ctx.branch_pane_resizing = false;
    }

    console_mouse_released(mouse_pos);
}

void app_scroll(double delta_x, double delta_y)
{
    console_scroll(delta_x, delta_y);
}

void app_window_resized(uint32_t width, uint32_t height)
{
    (void)width;
    (void)height;
    _ctx.branch_pane_rect.h = (int32_t)eva_get_window_height();
}

void app_draw(const eva_framebuffer *fb)
{
    profiler_begin;

    float font_size_pt = 16;

    const char *branches[] = {
        "master",
        "develop",
        "feature/AV",
        "pppppppppp",
        "ffffffffff",
        "Iñtërnâtiônàližætiøn",
        "Ἰοὺ ἰού· τὰ πάντʼ ἂν ἐξήκοι σαφῆ",
        "有子曰：「其為人也孝弟，而好犯上者，鮮矣",
    };

    vec2i padding = { 10, 10 };

    render_clear(&COLOR_LIGHT_GREY);
    render_draw_recti(&_ctx.branch_pane_rect, &COLOR_GREY);

    vec2i cursor = padding;
    for (size_t i = 0; i < array_size(branches); i++) {
        text *t = text_create_cstr(branches[i]);
        text_add_attr(t, 0, 0, FONT_FAMILY_MENLO, font_size_pt, &COLOR_WHITE);
        vec2i extents;
        text_extents(t, &extents);
        recti bbox = {
            .x = cursor.x,
            .y = cursor.y,
            .w = (int32_t)fb->h - cursor.x,
            .h = extents.y
        };
        recti clip = {
            .x = _ctx.branch_pane_rect.x,
            .y = _ctx.branch_pane_rect.y,
            .w = _ctx.branch_pane_rect.w - (padding.x * 2),
            .h = _ctx.branch_pane_rect.h
        };
        render_draw_text(t, &bbox, &clip);
        cursor.y += extents.y;
        text_destroy(t);
    }

    console_draw(fb);

    profiler_end;
}
