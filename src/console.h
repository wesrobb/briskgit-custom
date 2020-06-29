#pragma once

#include "common.h"

typedef struct eva_framebuffer eva_framebuffer;

void console_init();
void console_logn(char *text, size_t len);
void console_log(const char *fmt, ...);
void console_keydown(int32_t key, uint32_t mods);
void console_draw(const eva_framebuffer *);
