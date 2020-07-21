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
    console_init();

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
    if (_ctx.text_index + len < TEXT_MAX_LEN) {
        memcpy(_ctx.text + _ctx.text_index, text, len);
        _ctx.text_index += len;
        ustr str;
        str.data = (char*)text;
        str.len = _ctx.text_index;
        int32_t count = ustr_num_graphemes(&str);
        console_log("num graphemes %i", count);
        eva_request_frame();
    }
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
        puts("pane resizing");
    }

    console_mouse_pressed(mouse_pos);
}

void app_mouse_released(const vec2i *mouse_pos)
{
    if (_ctx.branch_pane_resizing) {
        _ctx.branch_pane_resizing = false;
        puts("pane stopped resizing");
    }

    console_mouse_released(mouse_pos);
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

    int32_t font_size_pt = 16;

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
    (void)light_grey;
    (void)grey;

    render_clear(&light_grey);
    render_draw_rect(&_ctx.branch_pane_rect, &grey);

    int32_t ascent, descent;
    render_get_font_height(FONT_ROBOTO_REGULAR, font_size_pt, 
                           &ascent, &descent);

    const char *text_lines[] = {
        "master",
        "develop",
        "feature/AV",
        "pppppppppp",
        "ffffffffff",
        "Iñtërnâtiônàližætiøn",
        "Ἰοὺ ἰού· τὰ πάντʼ ἂν ἐξήκοι σαφῆ",
        "有子曰：「其為人也孝弟，而好犯上者，鮮矣",
        "😁😂🤮",
        _ctx.text
    };

    uint32_t num_text_lines = sizeof(text_lines) / sizeof(text_lines[0]);
    for (int32_t i = 0; i < (int32_t)num_text_lines; i++) {
        const char *text = text_lines[i];

        size_t text_len = strlen(text);
        if ((uint32_t)i == num_text_lines - 1) {
            text_len = _ctx.text_index;
        }

        vec2i pos = {
            .x = 40,
            .y = (int32_t)(100 + (i * (ascent - descent)))
        };

        render_draw_font(FONT_ROBOTO_REGULAR,
                         text_lines[i], (int32_t)text_len, 
                         &pos, font_size_pt, &white);

        recti test = {
            .x = 0,
            .y = pos.y,
            .w = 900,
            .h = 2
        };
        render_draw_rect(&test, &white);
    }

    console_draw(fb);

    profiler_end;
}
