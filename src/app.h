#pragma once

#include "common.h"

typedef struct eva_framebuffer eva_framebuffer;

void app_init();
void app_shutdown();

void app_keydown(int32_t key, uint32_t mods);
void app_text_input(const char *text, uint32_t len);
void app_mouse_moved(int32_t x, int32_t y);
void app_mouse_pressed(int32_t x, int32_t y);
void app_mouse_released(int32_t x, int32_t y);
void app_window_resized(uint32_t window_width, uint32_t window_height);

void app_draw(const eva_framebuffer *);
