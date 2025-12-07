#pragma once

#include "cglm/types-struct.h"

typedef struct Chunk {
  ivec3s position;
} Chunk;

Chunk *Chunk_Init(ivec3s position);

void Chunk_Destroy(Chunk *chunk);
