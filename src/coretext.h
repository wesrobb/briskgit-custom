#pragma once

#include "common.h"

typedef struct eva_framebuffer eva_framebuffer;
typedef struct recti recti;
typedef struct text text;
typedef struct vec2i vec2i;

void coretext_draw_font(eva_framebuffer *fb, 
                        const recti *clip_rect,
                        char *text, int32_t len,
                        int32_t pt_size, const vec2i *pos);

void coretext_bounding_box(const text *t, recti *dst);
