#include "game.h"
#include "cglm/util.h"
#include "core/camera.h"
#include "world/world.h"
#include <assert.h>
#include <cglm/struct/cam.h>
#include <stdlib.h>
#include <time.h>

typedef struct Game {
  World *world;
  mat4s projection;

  Camera *camera;
} Game;

bool Game_Init(Game **out) {
  Game *game = malloc(sizeof(Game));

  srand(time(0));
  game->world = World_Init(rand());

  for (int z = -5; z < 5; ++z) {
    for (int x = -5; x < 5; ++x) {
      ivec3s position = {{x, 0, z}};
      World_EnsureChunk(game->world, position);

      ivec3s below = {{x, -1, z}};
      World_EnsureChunk(game->world, below);
    }
  }

  game->camera = Camera_Init((vec3s){{50.0f, 50.0f, 50.0f}});
  Camera_LookAt(game->camera, (vec3s){{0.0f, 0.0f, 0.0f}});

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  *out = game;
  return game;
}

void Game_Destroy(Game *game) {
  assert(game != NULL);

  if (game->world) {
    World_Destroy(game->world);
  }

  if (game->camera) {
    Camera_Destroy(game->camera);
  }

  free(game);
}

void Game_HandleResize(Game *game, int w, int h) {
  glViewport(0, 0, w, h);
  game->projection = glms_perspective(glm_rad(60.0f), (float)w / (float)h, 0.1f, 1000.0f);
}

void Game_HandleKeyboardEvent(Game *game, SDL_KeyboardEvent *event) {
  Camera_HandleKeyboardEvent(game->camera, event);
}

void Game_HandleMouseMotionEvent(Game *game, SDL_MouseMotionEvent *event) {
  Camera_HandleMouseMotion(game->camera, event);
}

void Game_Update(Game *game) {
}

void Game_Render(Game *game) {
  glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Camera_Update(game->camera);
  World_Update(game->world);

  World_Render(game->world, game->projection, game->camera);
}
