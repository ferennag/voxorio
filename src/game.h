#pragma once

#include "SDL3/SDL_events.h"
#include <GL/glew.h>

typedef struct Game Game;

bool Game_Init(Game **out);

void Game_Destroy(Game *game);

void Game_HandleResize(Game *game, int w, int h);

void Game_HandleKeyboardEvent(Game *game, SDL_KeyboardEvent *event);

void Game_HandleMouseMotionEvent(Game *game, SDL_MouseMotionEvent *event);

void Game_Update(Game *game);

void Game_Render(Game *game);
