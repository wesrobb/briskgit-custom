#pragma once

#include "common.h"

typedef struct SDL_Keysym SDL_Keysym;
typedef struct SDL_MouseMotionEvent SDL_MouseMotionEvent;
typedef struct SDL_MouseEvent SDL_MouseEvent;

void App_Init();
void App_Destroy();

void App_OnKeyPressed(SDL_Keysym *);
void App_OnKeyReleased(SDL_Keysym *);
void App_OnMouseMoved(SDL_MouseMotionEvent *);
void App_OnMousePressed(SDL_MouseEvent *);
void App_OnMouseReleased(SDL_MouseEvent *);
void App_OnWindowResized(int32_t width, int32_t height);

// TODO: Don't return the pixels this way.
void App_Draw();
