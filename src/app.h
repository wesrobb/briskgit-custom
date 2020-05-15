#pragma once

typedef struct SDL_Keysym SDL_Keysym;
typedef struct SDL_MouseMotionEvent SDL_MouseMotionEvent;
typedef struct SDL_MouseEvent SDL_MouseEvent;
typedef struct SDL_Surface SDL_Surface;

void App_Init();
void App_OnKeyPressed(SDL_Keysym *);
void App_OnKeyReleased(SDL_Keysym *);
void App_OnMouseMoved(SDL_MouseMotionEvent *);
void App_OnMousePressed(SDL_MouseEvent *);
void App_OnMouseReleased(SDL_MouseEvent *);
void App_OnWindowResized(int width, int height);
void App_Draw(SDL_Surface *, float dpiX, float dpiY);
