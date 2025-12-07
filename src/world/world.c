#include "world.h"
#include "chunk.h"
#include "chunk_hashmap.h"
#include "core/shader.h"
#include <assert.h>
#include <stdlib.h>

typedef struct World {
  int seed;
  ChunkHashMap chunks;
  Shader *shader;
} World;

World *World_Init(int seed) {
  World *world = malloc(sizeof(World));
  ChunkHashMap_Init(&world->chunks);
  world->shader = Shader_load("assets/shaders/basic.vert", "assets/shaders/basic.frag");
  world->seed = seed;

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

  if (world->shader) {
    Shader_Destroy(world->shader);
  }

  ChunkHashMap_Destroy(&world->chunks);
  free(world);
}

void World_Update(World *world) {
}

void World_Render(World *world, mat4s projection, mat4s view) {
  Shader_Bind(world->shader);
  Shader_UniformMat4(world->shader, "projection", projection);
  Shader_UniformMat4(world->shader, "view", view);

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
