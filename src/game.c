#include "game.h"
#include "core/shader.h"
#include "world/world.h"
#include <assert.h>
#include <stdlib.h>

typedef struct Game {
  Shader *shader;
  GLuint vao, vbo;

  World *world;
} Game;

static float vertices[][3] = {
    {-0.5f, -0.5f, 0.0f},
    {0.5f, -0.5f, 0.0f},
    {0.0f, 0.5f, 0.0f},
};

bool Game_Init(Game **out) {
  Shader *shader = Shader_load("assets/shaders/basic.vert", "assets/shaders/basic.frag");
  if (!shader) {
    return false;
  }

  Game *game = malloc(sizeof(Game));
  game->shader = shader;
  game->world = World_Init();

  for (int z = -1; z < 1; ++z) {
    for (int x = -1; x < 1; ++x) {
      ivec3s position = {{x, 0, z}};
      World_EnsureChunk(game->world, position);
    }
  }

  glCreateVertexArrays(1, &game->vao);
  glCreateBuffers(1, &game->vbo);
  glBindVertexArray(game->vao);
  glBindBuffer(GL_ARRAY_BUFFER, game->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
  glEnableVertexAttribArray(0);

  *out = game;
  return game;
}

void Game_Destroy(Game *game) {
  assert(game != NULL);

  if (game->shader) {
    Shader_Destroy(game->shader);
  }

  if (game->world) {
    World_Destroy(game->world);
  }

  free(game);
}

void Game_HandleResize(Game *game, int w, int h) {
  glViewport(0, 0, w, h);
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

  Shader_Bind(game->shader);
  glBindVertexArray(game->vao);
  glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));
  Shader_Unbind(game->shader);
}
