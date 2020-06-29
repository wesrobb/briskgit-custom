#pragma once

#include "common.h"

typedef struct eva_framebuffer eva_framebuffer;

void console_init();
void console_logn(char *text, size_t len);
void console_log(const char *fmt, ...);
void console_mouse_moved(int32_t x, int32_t y);
void console_mouse_pressed(int32_t x, int32_t y);
void console_mouse_released(int32_t x, int32_t y);
void console_keydown(int32_t key, uint32_t mods);
void console_draw(const eva_framebuffer *);
