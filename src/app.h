#pragma once

#include "common.h"

typedef struct eva_mouse_event eva_mouse_event;

void app_init();
void app_shutdown();

void app_keydown(int32_t key);
void app_mouse_moved(int32_t x, int32_t y);
void app_mouse_pressed(int32_t x, int32_t y);
void app_mouse_released(int32_t x, int32_t y);
void app_window_resized(uint32_t window_width, uint32_t window_height);

void app_draw();
