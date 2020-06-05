#include "app.h"

#include "eva/eva.h"

#include "profiler.h"
#include "render.h"

#include <stdio.h>

typedef struct app_ctx {
    int32_t branch_pane_resize_range;
    eva_rect branch_pane_rect;
    bool branch_pane_resizing;
    int32_t branch_pane_min_size;

} app_ctx;

static app_ctx _app_ctx;

static bool point_in_rect(int32_t x, int32_t y, eva_rect *r)
{
    return x >= r->x          &&
           x <= (r->x + r->w) &&
           y >= r->y          &&
           y <= (r->y + r->h);
}

void app_init()
{
    float scale_x = eva_get_framebuffer_scale_x();

    _app_ctx.branch_pane_resize_range = (int32_t)(5 * scale_x);
    _app_ctx.branch_pane_rect.w = 400;
    _app_ctx.branch_pane_rect.h = eva_get_window_height();
    _app_ctx.branch_pane_min_size = 200;
}

void app_shutdown()
{
}

void app_mouse_moved(eva_mouse_event *e)
{
    if (_app_ctx.branch_pane_resizing) {
        _app_ctx.branch_pane_rect.w =
            max(e->mouse_x, _app_ctx.branch_pane_min_size);
    }

    eva_rect resizeHandle = {
        .x = _app_ctx.branch_pane_rect.x + _app_ctx.branch_pane_rect.w -
             _app_ctx.branch_pane_resize_range,
        .y = _app_ctx.branch_pane_rect.y,
        .w = _app_ctx.branch_pane_resize_range * 2,
        .h = _app_ctx.branch_pane_rect.y + _app_ctx.branch_pane_rect.h,
    };
    (void)resizeHandle;
}

void app_mouse_pressed(eva_mouse_event *e)
{
    (void)e;
    if (e->left_button_pressed) {
        eva_rect resizeHandle = {
            .x = _app_ctx.branch_pane_rect.x + _app_ctx.branch_pane_rect.w -
                 _app_ctx.branch_pane_resize_range,
            .y = _app_ctx.branch_pane_rect.y,
            .w = _app_ctx.branch_pane_resize_range * 2,
            .h = _app_ctx.branch_pane_rect.y + _app_ctx.branch_pane_rect.h,
        };

        if (point_in_rect(e->mouse_x, e->mouse_y, &resizeHandle)) {
            _app_ctx.branch_pane_resizing = true;
        }
    }
}

void app_mouse_released(eva_mouse_event *e)
{
    (void)e;
    if (e->left_button_released) {
        if (_app_ctx.branch_pane_resizing) {
            _app_ctx.branch_pane_resizing = false;
        }
    }
}

void app_window_resized(int32_t width, int32_t height)
{
    (void)width;
    _app_ctx.branch_pane_rect.h = height;
}

void app_draw()
{
    profiler_begin;

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

    color white = {
        .r = 1.0f,
        .g = 1.0f,
        .b = 1.0f,
        .a = 1.0f,
    };

    render_clear(light_grey);
    render_draw_rect(&_app_ctx.branch_pane_rect, grey);
    // Render_DrawHollowRect(testRect, white, 4);

    int32_t font_size_pt = 18;
    // int32_t ascent, descent;
    // Render_GetFontHeight(FONT_ROBOTO_REGULAR, fontSizePt, &ascent, &descent);

    const char *text_lines[] = {
        "master",
        "develop",
        "feature/AV",
        "pppppppppp",
        "ffffffffff"
    };

    uint32_t num_text_lines = sizeof(text_lines) / sizeof(text_lines[0]);
    for (uint32_t i = 0; i < num_text_lines; i++) {
        // int32_t width = Render_GetTextWidth(FONT_ROBOTO_REGULAR,
        // textLines[i], fontSizePt);
        int32_t x = 10;
        int32_t y = (int32_t)(20 + (i * (22)));
        // Rect r = {
        //    .x = x,
        //    .y = y - ascent,
        //    .w = width,
        //    .h = ascent - descent
        //};

        // Render_DrawHollowRect(r, white, 2);
        render_draw_font(FONT_ROBOTO_REGULAR,
                         text_lines[i], 
                         x, y, font_size_pt, white);
    }

    profiler_end;
}
