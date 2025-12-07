#include "world.h"
#include "SDL3/SDL_surface.h"
#include "cglm/struct/vec3.h"
#include "chunk.h"
#include "chunk_hashmap.h"
#include "core/camera.h"
#include "core/shader.h"
#include <GL/glew.h>
#include <SDL3_image/SDL_image.h>
#include <assert.h>
#include <stdlib.h>

typedef struct World {
  int seed;
  ChunkHashMap chunks;
  Shader *shader;
  GLuint texture;
} World;

World *World_Init(int seed) {
  World *world = malloc(sizeof(World));
  ChunkHashMap_Init(&world->chunks);
  world->shader = Shader_load("assets/shaders/basic.vert", "assets/shaders/basic.frag");
  world->seed = seed;
  glCreateTextures(GL_TEXTURE_2D, 1, &world->texture);
  glBindTexture(GL_TEXTURE_2D, world->texture);
  SDL_Surface *dirt = IMG_Load("assets/textures/dirt.png");
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dirt->w, dirt->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, dirt->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_DestroySurface(dirt);

  return world;
}

void World_EnsureChunk(World *world, ivec3s position) {
  Chunk *chunk = ChunkHashMap_Find(&world->chunks, position);
  if (chunk != NULL) {
    return;
  }

  chunk = Chunk_Init(world->seed, position);
  ChunkHashMap_Add(&world->chunks, chunk);
  Chunk_Generate(chunk);
}

void World_Destroy(World *world) {
  assert(world != NULL);

  glDeleteTextures(1, &world->texture);
  if (world->shader) {
    Shader_Destroy(world->shader);
  }

  ChunkHashMap_Destroy(&world->chunks);
  free(world);
}

void World_Update(World *world) {
}

void World_Render(World *world, mat4s projection, Camera *camera) {
  Shader_Bind(world->shader);
  Shader_UniformMat4(world->shader, "projection", projection);
  Shader_UniformMat4(world->shader, "view", Camera_ViewMat(camera));

  vec3s sunPosition = {{2.0f, 1.0f, 2.0f}};
  vec3s sunDirection = glms_vec3_sub((vec3s){{0.0f, 0.0f, 0.0f}}, sunPosition);
  Shader_UniformVec3(world->shader, "sunDirection", sunDirection);
  Shader_UniformVec3(world->shader, "sunColor", (vec3s){{1.0f, 1.0f, 1.0f}});
  Shader_UniformVec3(world->shader, "eye", Camera_GetPosition(camera));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, world->texture);

  for (int i = 0; i < world->chunks.size; ++i) {
    ChunkNode *node = world->chunks.nodes[i];

    while (node != NULL) {
      if (node->chunk != NULL) {
        Shader_UniformMat4(world->shader, "model", Chunk_ModelMat(node->chunk));
        Chunk_Render(node->chunk);
      }
      node = node->next;
    }
  }

  Shader_Unbind(world->shader);
}
