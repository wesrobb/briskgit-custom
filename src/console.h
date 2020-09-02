#pragma once

#include "common.h"

typedef struct eva_framebuffer eva_framebuffer;
typedef struct vec2 vec2;

void console_init();
void console_logn(char *text, size_t len);
void console_log(const char *fmt, ...);
void console_mouse_moved(const vec2 *mouse_pos);
void console_mouse_pressed(const vec2 *mouse_pos);
void console_mouse_released(const vec2 *mouse_pos);
void console_scroll(double delta_x, double delta_y);
void console_keydown(int32_t key, uint32_t mods);
void console_draw(const eva_framebuffer *);
