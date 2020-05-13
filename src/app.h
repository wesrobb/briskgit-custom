#pragma once

typedef struct SDL_KeyEvent SDL_KeyEvent;
typedef struct SDL_MouseEvent SDL_MouseEvent;
typedef struct SDL_Surface SDL_Surface;

void App_OnKeyPressed(SDL_KeyEvent *);
void App_OnKeyReleased(SDL_KeyEvent *);
void App_OnMouseMoved(SDL_MouseEvent *);
void App_OnMousePressed(SDL_MouseEvent *);
void App_OnMouseReleased(SDL_MouseEvent *);
void App_Draw(SDL_Surface *);
