#include "console.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "eva/eva.h"

#include "render.h"

#define MAX_LOG_ENTRIES 2048

typedef struct log_entry {
    char *text;
    size_t len;
} log_entry;

typedef struct log_entries {
    log_entry entries[MAX_LOG_ENTRIES];
    int32_t start;
    int32_t count;
} log_entries;

typedef struct console_ctx {
    bool visible;
    eva_rect rect;

    log_entries logs;
} console_ctx;

typedef struct scrollbar {
    float content_size;
    float window_size;
    float track_size;

    float window_content_ratio;
    float grip_size;
    float min_grip_size;

    float window_scroll_area_size;
    float window_pos;
    float window_pos_ratio;
    float track_scroll_area_size;
    float grip_pos_on_track;
} scrollbar;

static console_ctx _ctx;

void console_init()
{
}

void console_logn(char *text, size_t len)
{
    assert(text);
    assert(len > 0);

    log_entry *entry;
    if (_ctx.logs.count == MAX_LOG_ENTRIES) {
        entry = &_ctx.logs.entries[_ctx.logs.start++];
        _ctx.logs.start = _ctx.logs.start % MAX_LOG_ENTRIES;
    }
    else {
        entry = &_ctx.logs.entries[_ctx.logs.count++];
        _ctx.logs.count = _ctx.logs.count % MAX_LOG_ENTRIES;
    }

    // Only free and malloc if the buffer we have is not big enough for the
    // new message;
    if (!entry->text) {
        entry->text = malloc(len);
    }
    else if (entry->text && entry->len < len) {
        free(entry->text);
        entry->text = malloc(len);
    }

    entry->len = len;

    memcpy(entry->text, text, len);
}

void console_log(const char *fmt, ...)
{
    assert(fmt);

    log_entry *entry;
    if (_ctx.logs.count == MAX_LOG_ENTRIES) {
        entry = &_ctx.logs.entries[_ctx.logs.start++];
        _ctx.logs.start = _ctx.logs.start % MAX_LOG_ENTRIES;
    }
    else {
        entry = &_ctx.logs.entries[_ctx.logs.count++];
        _ctx.logs.count = _ctx.logs.count % MAX_LOG_ENTRIES;
    }

    va_list args;
    va_start(args, fmt);
    size_t text_len = (size_t)vsnprintf(entry->text, 0, fmt, args);
    text_len += 1; // Make space for the null terminator

    // Only free and malloc if the buffer we have is not big enough for the
    // new message;
    if (!entry->text) {
        entry->text = malloc(text_len);
    }
    else if (entry->text && entry->len < text_len) {
        free(entry->text);
        entry->text = malloc(text_len);
    }

    entry->len = text_len;
    vsnprintf(entry->text, text_len, fmt, args);
    va_end(args);
}

void console_mouse_moved(int32_t x, int32_t y)
{
}

void console_mouse_pressed(int32_t x, int32_t y)
{
}

void console_mouse_released(int32_t x, int32_t y)
{
}

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

        color white = {
            .r = 1.0f,
            .g = 1.0f,
            .b = 1.0f,
            .a = 1.0f,
        };

        color dark_blue = {
            .r = 0.0f,
            .g = 0.0f,
            .b = 0.7f,
            .a = 1.0f,
        };

        eva_rect rect = {
            .x = 0,
            .y = 0,
            .w = (int32_t)fb->w,
            .h = (int32_t)(fb->h / 3.0f)
        };
        render_draw_rect(&rect, base_color);

        int32_t pt_size = 12;
        int32_t padding = 10;
        int32_t ascent, descent;
        render_get_font_height(FONT_ROBOTO_REGULAR, pt_size, &ascent, &descent);
        int32_t font_height = ascent - descent;

        int32_t line_gap = 0;
        int32_t max_rows = rect.h / (line_gap + font_height);

        scrollbar sb;
        sb.content_size = _ctx.logs.count * font_height;
        sb.window_size = rect.h;
        sb.track_size = rect.h;
        sb.window_content_ratio = sb.window_size / sb.content_size;
        sb.grip_size = sb.track_size * sb.window_content_ratio;
        sb.min_grip_size = 20.0f;

        sb.grip_size = max(sb.min_grip_size, sb.grip_size);
        sb.grip_size = min(sb.track_size, sb.grip_size);
        
        sb.window_scroll_area_size = sb.content_size - sb.window_size;
        sb.window_pos = 100.0f; // TODO: ??????
        sb.window_pos_ratio = sb.window_pos / sb.window_scroll_area_size;
        sb.track_scroll_area_size = sb.track_size - sb.grip_size;
        sb.grip_pos_on_track = sb.track_scroll_area_size * sb.window_pos_ratio;

        int32_t track_center = (int32_t)fb->w - padding;
        int32_t track_width = 5;
        eva_rect track_rect = {
            .x = (int32_t)(track_center - track_width / 2.0f),
            .y = rect.y,
            .w = track_width,
            .h = (int32_t)sb.track_size
        };

        int32_t grip_width = 10;
        eva_rect grip_rect = {
            .x = (int32_t)(track_center - (grip_width / 2.0f)),
            .y = (int32_t)sb.grip_pos_on_track,
            .w = grip_width,
            .h = (int32_t)sb.grip_size,
        };

        render_draw_rect(&track_rect, dark_blue);
        render_draw_rect(&grip_rect, white);

        // Work backwards through the entries till we have enough to fill the
        // available space in the console window.
        int32_t log_count = _ctx.logs.count;
        int32_t row_count = 0;
        int32_t text_pos_x = rect.x + padding;
        int32_t text_pos_y = rect.y + rect.h + padding + descent;
        while (row_count <= max_rows && 
               log_count > 0 && 
               text_pos_y > font_height + padding) {
            int32_t index = (_ctx.logs.start + --log_count) % MAX_LOG_ENTRIES;
            log_entry *entry = &_ctx.logs.entries[index];
            text_pos_y -= font_height;

            render_draw_font(FONT_ROBOTO_REGULAR,
                             (const char*)entry->text, (int32_t)entry->len,
                             text_pos_x, text_pos_y, pt_size, white);

            row_count++;
        }
    }
}
