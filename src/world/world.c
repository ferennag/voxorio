#include "world.h"
#include "chunk.h"
#include "chunk_hashmap.h"
#include <assert.h>
#include <stdlib.h>

typedef struct World {
  ChunkHashMap chunks;
} World;

World *World_Init(void) {
  World *world = malloc(sizeof(World));
  ChunkHashMap_Init(&world->chunks);
  return world;
}

void World_EnsureChunk(World *world, ivec3s position) {
  Chunk *chunk = ChunkHashMap_Find(&world->chunks, position);
  if (chunk != NULL) {
    return;
  }

  chunk = Chunk_Init(position);
  ChunkHashMap_Add(&world->chunks, chunk);
}

void World_Destroy(World *world) {
  assert(world != NULL);
  ChunkHashMap_Destroy(&world->chunks);
  free(world);
}

void World_Update(World *world) {
}

void World_Render(World *world) {
}
