#include "console.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "eva/eva.h"

#include "render.h"

#define MAX_LOG_ENTRIES 2048

typedef struct log_entry {
    uint8_t *text;
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

static console_ctx _ctx;

void console_init()
{
    _ctx.logs.start = 0;
    _ctx.logs.count = 0;
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

void console_log(const char *text)
{
    assert(text);

    log_entry *entry;
    if (_ctx.logs.count == MAX_LOG_ENTRIES) {
        entry = &_ctx.logs.entries[_ctx.logs.start++];
        _ctx.logs.start = _ctx.logs.start % MAX_LOG_ENTRIES;
    }
    else {
        entry = &_ctx.logs.entries[_ctx.logs.count++];
        _ctx.logs.count = _ctx.logs.count % MAX_LOG_ENTRIES;
    }

    size_t text_len = strlen(text);

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

    memcpy(entry->text, text, text_len);
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

        eva_rect rect = {
            .x = 0,
            .y = 0,
            .w = (int32_t)fb->w,
            .h = (int32_t)(fb->h / 3.0f)
        };
        render_draw_rect(&rect, base_color);

        int32_t pt_size = 12;
        int32_t padding = 5;
        int32_t ascent, descent;
        render_get_font_height(FONT_ROBOTO_REGULAR, pt_size, &ascent, &descent);
        int32_t font_height = ascent - descent;

        int32_t line_gap = 0;
        int32_t max_rows = rect.h / (line_gap + font_height);


        // Work backwards through the entries till we have enough to fill the
        // available space in the console window.
        int32_t log_count = _ctx.logs.count;
        int32_t row_count = 0;
        int32_t text_pos_x = rect.x + padding;
        int32_t text_pos_y = rect.y + rect.h + padding + descent;
        while (row_count <= max_rows && log_count > 0) {
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
