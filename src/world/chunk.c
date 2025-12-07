#include "chunk.h"
#include "SDL3/SDL_log.h"
#include <assert.h>
#include <stdlib.h>

Chunk *Chunk_Init(ivec3s position) {
  Chunk *chunk = malloc(sizeof(Chunk));
  chunk->position = position;
  SDL_Log("Creating chunk: (%d, %d, %d)", chunk->position.x, chunk->position.y, chunk->position.z);
  return chunk;
}

void Chunk_Destroy(Chunk *chunk) {
  assert(chunk != NULL);

  SDL_Log("Destroying chunk: (%d, %d, %d)", chunk->position.x, chunk->position.y, chunk->position.z);
  free(chunk);
}
