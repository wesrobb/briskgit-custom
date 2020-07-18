#pragma once

#include "common.h"

typedef struct color color;
typedef struct recti recti;
typedef struct vec2i vec2i;

typedef enum font {
    FONT_ROBOTO_REGULAR,
    FONT_MENLO_REGULAR,
    FONT_COUNT
} font;

bool render_init(void);
void render_shutdown(void);

void render_begin_frame(void);
void render_end_frame(void);

void render_clear(const color *c);
void render_draw_rect(const recti *r, const color *c);
void render_draw_hollow_rect(const recti *r, const color *c,
                             int32_t border_thickness);
void render_draw_font(font font, const char *text, int32_t text_len,
                      const vec2i *pos, int32_t pt_size, const color *c);

int32_t render_get_text_width(font font, const char *text, int32_t text_len, 
                              int32_t pt_size);
void    render_get_font_height(font font, int32_t pt_size,
                               int32_t *ascent, int32_t *descent);
