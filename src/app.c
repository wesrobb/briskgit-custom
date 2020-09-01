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
#define MAX_BRANCHES 64

typedef struct app_ctx {
    float branch_pane_resize_range;
    rectf branch_pane_rect;
    bool branch_pane_resizing;
    float branch_pane_min_size;

    text *branches[MAX_BRANCHES];
    vec2f text_positions[MAX_BRANCHES];
    int32_t num_branches;

    char text[TEXT_MAX_LEN];
    size_t text_index;

    bool text_select;
    size_t start_index;
    size_t end_index;
} app_ctx;

static app_ctx _ctx;

void app_init()
{
    eva_framebuffer fb = eva_get_framebuffer();

    _ctx.branch_pane_resize_range = (int32_t)(5 * fb.scale_x);
    _ctx.branch_pane_rect.w = 400;
    _ctx.branch_pane_rect.h = (int32_t)fb.h;
    _ctx.branch_pane_min_size = 200;

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

    _ctx.num_branches = array_size(branches);
    float font_size_pt = 14.0f;
    for (int32_t i = 0; i < _ctx.num_branches; i++) {
        text *t = text_create_cstr(branches[i]);
        _ctx.branches[i] = t;
        text_add_attr(t, 0, 0, FONT_FAMILY_MENLO, font_size_pt, &COLOR_WHITE);
    }

    // Cache text position for hit testing on mouse move.
    vec2f padding = { 10, 10 };
    vec2f cursor = padding;
    for (int32_t i = 0; i < _ctx.num_branches; i++) {
        text *t = _ctx.branches[i];
        vec2f extents;
        text_extents(t, &extents);
        _ctx.text_positions[i] = cursor;
        cursor.y += extents.y;
    }
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

void app_mouse_moved(const vec2f *mouse_pos)
{
    if (_ctx.text_select) {
        size_t str_index = 0;
        vec2f pos = _ctx.text_positions[0];
        vec2f mouse_pos_in_text_coords = vec2f_sub(mouse_pos, &pos);
        bool hit = text_hit(_ctx.branches[0], &mouse_pos_in_text_coords,
                            &str_index);
        if (hit) {
            _ctx.end_index = str_index;
            printf("Hits string index %zu\n", str_index);
            eva_request_frame();
        }
    }

    if (_ctx.branch_pane_resizing) {
        _ctx.branch_pane_rect.w = max(mouse_pos->x, _ctx.branch_pane_min_size);
        eva_request_frame();
    }

    rectf resizeHandle = {
        .x = _ctx.branch_pane_rect.x + _ctx.branch_pane_rect.w -
             _ctx.branch_pane_resize_range,
        .y = _ctx.branch_pane_rect.y,
        .w = _ctx.branch_pane_resize_range * 2,
        .h = _ctx.branch_pane_rect.y + _ctx.branch_pane_rect.h,
    };
    (void)resizeHandle;

    console_mouse_moved(mouse_pos);
}

void app_mouse_pressed(const vec2f *mouse_pos)
{
    if (!_ctx.text_select) {
        size_t str_index = 0;
        vec2f pos = _ctx.text_positions[0];
        vec2f mouse_pos_in_text_coords = vec2f_sub(mouse_pos, &pos);
        bool hit = text_hit(_ctx.branches[0], &mouse_pos_in_text_coords,
                            &str_index);
        if (hit) {
            _ctx.text_select = true;
            _ctx.start_index = str_index;
            _ctx.end_index = str_index;
            printf("Started hit string index %zu\n", str_index);
            eva_request_frame();
        }
    }

    rectf resizeHandle = {
        .x = _ctx.branch_pane_rect.x + _ctx.branch_pane_rect.w -
            _ctx.branch_pane_resize_range,
        .y = _ctx.branch_pane_rect.y,
        .w = _ctx.branch_pane_resize_range * 2,
        .h = _ctx.branch_pane_rect.y + _ctx.branch_pane_rect.h,
    };

    if (rectf_point_intersect(&resizeHandle, mouse_pos)) {
        _ctx.branch_pane_resizing = true;
    }

    console_mouse_pressed(mouse_pos);
}

void app_mouse_released(const vec2f *mouse_pos)
{
    if (_ctx.text_select) {
        size_t str_index = 0;
        vec2f pos = _ctx.text_positions[0];
        vec2f mouse_pos_in_text_coords = vec2f_sub(mouse_pos, &pos);
        bool hit = text_hit(_ctx.branches[0], &mouse_pos_in_text_coords,
                            &str_index);
        if (hit) {
            _ctx.text_select = false;
            _ctx.end_index = str_index;
            printf("Ended hit string index %zu\n", str_index);
            eva_request_frame();
        }
    }

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

    vec2f padding = { 10, 10 };

    render_clear(&COLOR_LIGHT_GREY);
    render_draw_rectf(&_ctx.branch_pane_rect, &COLOR_GREY);

    for (int32_t i = 0; i < _ctx.num_branches; i++) {
        text *t = _ctx.branches[i];
        vec2f *pos = &_ctx.text_positions[i];
        vec2f extents;
        text_extents(t, &extents);

        if (i == 0 && _ctx.text_select) {
            float offset_a = text_index_offset(t, _ctx.start_index);
            float offset_b = text_index_offset(t, _ctx.end_index);
            float start_offset = min(offset_a, offset_b);
            float end_offset = max(offset_a, offset_b);
            float width = end_offset - start_offset;
            rectf highlight = {
                .x = pos->x + start_offset,
                .y = pos->y,
                .w = width,
                .h = extents.y,
            };
            render_draw_rectf(&highlight, &COLOR_LIGHT_BLUE);
        }


        rectf bbox = {
            .x = pos->x,
            .y = pos->y,
            .w = _ctx.branch_pane_rect.w - (padding.x * 2),
            .h = extents.y
        };
        rectf clip = {
            .x = _ctx.branch_pane_rect.x,
            .y = _ctx.branch_pane_rect.y,
            .w = _ctx.branch_pane_rect.w - (padding.x * 2),
            .h = _ctx.branch_pane_rect.h
        };

        render_draw_text(t, &bbox, &clip);
    }

    console_draw(fb);

    profiler_end;
}
