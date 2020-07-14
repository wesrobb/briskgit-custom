#pragma once

#include "common.h"

typedef struct eva_framebuffer eva_framebuffer;
typedef struct eva_rect eva_rect;

void coretext_draw_font(eva_framebuffer *fb, 
                        eva_rect *clip_rect,
                        char *text, int32_t len,
                        int32_t pt_size, int32_t x, int32_t y);
