#include "chunk_hashmap.h"
#include "cglm/struct/ivec3.h"
#include <stdlib.h>

int ChunkHashMap_Hash(ChunkHashMap *map, ivec3s position) {
  return 0;
}

void ChunkHashMap_Init(ChunkHashMap *dst) {
  dst->size = 100;
  dst->nodes = calloc(dst->size, sizeof(ChunkNode));
}

void ChunkHashMap_Destroy(ChunkHashMap *hashMap) {
  for (int i = 0; i < hashMap->size; ++i) {
    ChunkNode *node = hashMap->nodes[i];

    while (node) {
      if (node->chunk) {
        Chunk_Destroy(node->chunk);
      }

      ChunkNode *next = node->next;
      free(node);
      node = next;
    }
  }

  free(hashMap->nodes);
}

void ChunkHashMap_Add(ChunkHashMap *map, Chunk *chunk) {
  int hash = ChunkHashMap_Hash(map, Chunk_GetPosition(chunk));
  ChunkNode *node = map->nodes[hash];

  if (node == NULL) {
    node = calloc(1, sizeof(ChunkNode));
    map->nodes[hash] = node;
  }

  while (node->chunk != NULL) {
    if (node->next == NULL) {
      node->next = calloc(1, sizeof(ChunkNode));
    }

    node = node->next;
  }

  node->chunk = chunk;
}

Chunk *ChunkHashMap_Find(ChunkHashMap *map, ivec3s position) {
  int hash = ChunkHashMap_Hash(map, position);
  ChunkNode *node = map->nodes[hash];

  if (node == NULL) {
    return NULL;
  }

  while (node != NULL) {
    if (glms_ivec3_eqv(Chunk_GetPosition(node->chunk), position)) {
      return node->chunk;
    }
    node = node->next;
  }

  return NULL;
}
