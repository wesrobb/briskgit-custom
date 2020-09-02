#pragma once

#include "common.h"

typedef struct color color;
typedef struct rect rect;
typedef struct recti recti;
typedef struct text text;
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
void render_draw_rect(const rect *r, const color *c);
void render_draw_recti(const recti *r, const color *c);
void render_draw_text(text *t, const rect *bbox, const rect *clip);
