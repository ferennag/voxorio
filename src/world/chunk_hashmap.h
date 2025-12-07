#pragma once

#include "chunk.h"

typedef struct ChunkNode {
  Chunk *chunk;
  struct ChunkNode *next;
} ChunkNode;

typedef struct ChunkHashMap {
  ChunkNode **nodes;
  unsigned int size;
} ChunkHashMap;

int ChunkHashMap_Hash(ChunkHashMap *map, ivec3s position);

void ChunkHashMap_Init(ChunkHashMap *dst);

void ChunkHashMap_Destroy(ChunkHashMap *hashMap);

void ChunkHashMap_Add(ChunkHashMap *map, Chunk *chunk);

Chunk *ChunkHashMap_Find(ChunkHashMap *map, ivec3s position);
