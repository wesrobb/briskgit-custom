#pragma once

#include "common.h"

typedef struct eva_mouse_event eva_mouse_event;

void App_Init();
void App_Destroy();

// void App_OnKeyPressed(SDL_Keysym *);
// void App_OnKeyReleased(SDL_Keysym *);
void App_OnMouseMoved(eva_mouse_event *);
void App_OnMousePressed(eva_mouse_event *);
void App_OnMouseReleased(eva_mouse_event *);
void App_OnWindowResized(int32_t width, int32_t height);

void App_Draw();
