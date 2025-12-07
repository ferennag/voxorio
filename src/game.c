#include "game.h"
#include "cglm/util.h"
#include "world/world.h"
#include <assert.h>
#include <cglm/struct/cam.h>
#include <stdlib.h>

typedef struct Game {
  World *world;
  mat4s projection, view;
} Game;

bool Game_Init(Game **out) {
  Game *game = malloc(sizeof(Game));
  game->world = World_Init();

  for (int z = -1; z < 1; ++z) {
    for (int x = -1; x < 1; ++x) {
      ivec3s position = {{x, 0, z}};
      World_EnsureChunk(game->world, position);
    }
  }

  game->view = glms_lookat((vec3s){{100.0f, 100.0f, 100.0f}}, (vec3s){{0.0f, 0.0f, 0.0f}}, (vec3s){{0.0f, 1.0f, 0.0f}});

  *out = game;
  return game;
}

void Game_Destroy(Game *game) {
  assert(game != NULL);

  if (game->world) {
    World_Destroy(game->world);
  }

  free(game);
}

void Game_HandleResize(Game *game, int w, int h) {
  glViewport(0, 0, w, h);
  game->projection = glms_perspective(glm_rad(60.0f), (float)w / (float)h, 0.1f, 1000.0f);
}

void Game_HandleKeyboardEvent(Game *game, SDL_KeyboardEvent *event) {
}

void Game_HandleMouseMotionEvent(Game *game, SDL_MouseMotionEvent *event) {
}

void Game_Update(Game *game) {
}

void Game_Render(Game *game) {
  glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  World_Render(game->world, game->projection, game->view);
}
