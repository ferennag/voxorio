#include "SDL3/SDL_video.h"
#include "game.h"
#define SDL_MAIN_USE_CALLBACKS 1
#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>

typedef struct GameState {
  SDL_Window *window;
  SDL_GLContext context;
  Game *game;
} GameState;

SDL_AppResult SDL_AppInit(void **state, int argc, char **argv) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Error initializing SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

  SDL_Window *window =
      SDL_CreateWindow("Voxorio", 1024, 768, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_OPENGL);
  if (!window) {
    SDL_Log("Error creating SDL window: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_GLContext context = SDL_GL_CreateContext(window);
  if (!context) {
    SDL_Log("Error creating OpenGL context: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_GL_MakeCurrent(window, context);

  glewInit();

  GameState *gameState = malloc(sizeof(GameState));
  gameState->window = window;
  gameState->context = context;
  if (!Game_Init(&gameState->game)) {
    return SDL_APP_FAILURE;
  }

  *state = gameState;

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *state, SDL_Event *event) {
  GameState *gameState = (GameState *)state;
  switch (event->type) {
    case SDL_EVENT_QUIT: {
      return SDL_APP_SUCCESS;
    }
    case SDL_EVENT_WINDOW_RESIZED: {
      Game_HandleResize(gameState->game, event->window.data1, event->window.data2);
      break;
    }
    case SDL_EVENT_MOUSE_MOTION: {
      Game_HandleMouseMotionEvent(gameState->game, &event->motion);
      break;
    }
    case SDL_EVENT_KEY_UP:
    case SDL_EVENT_KEY_DOWN: {
      Game_HandleKeyboardEvent(gameState->game, &event->key);
      switch (event->key.key) {
        case SDLK_ESCAPE: {
          return SDL_APP_SUCCESS;
        }
      }
      break;
    }
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *state) {
  GameState *gameState = (GameState *)state;
  Game_Update(gameState->game);
  Game_Render(gameState->game);
  SDL_GL_SwapWindow(gameState->window);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *state, SDL_AppResult result) {
  GameState *gameState = (GameState *)state;

  if (gameState->game) {
    Game_Destroy(gameState->game);
  }

  free(gameState);
}
