#pragma once

#include "common.h"

typedef struct eva_mouse_event eva_mouse_event;

void app_init();
void app_shutdown();

void app_keydown(const char* utf8_codepoint);
void app_mouse_moved(eva_mouse_event *);
void app_mouse_pressed(eva_mouse_event *);
void app_mouse_released(eva_mouse_event *);
void app_window_resized(int32_t width, int32_t height);

void app_draw();
