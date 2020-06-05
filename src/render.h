#pragma once

#include "common.h"

typedef struct eva_pixel eva_pixel;
typedef struct eva_rect  eva_rect;

typedef struct color {
    float b, g, r, a;
} color;

typedef enum font {
    FONT_ROBOTO_REGULAR,
    FONT_MENLO_REGULAR,
    FONT_COUNT
} font;

bool render_init();
void render_shutdown();

void render_begin_frame();
void render_end_frame(eva_rect *dirty_rect);

void render_clear(color);
void render_draw_rect(eva_rect *, color);
void render_draw_hollow_rect(eva_rect *rect, color, int32_t border_thickness);
void render_draw_font(font font, const char *text,
                      int32_t pos_x, int32_t pos_y,
                      int32_t pt_size, color);

int32_t render_get_text_width(font font, const char *text, int32_t pt_size);
void    render_get_font_height(font font, int32_t pt_size,
                               int32_t *ascent, int32_t *descent);
