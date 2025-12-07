#pragma once

#include "cglm/types-struct.h"
#include <stdbool.h>
#include <stdint.h>

typedef uint64_t u64;

static const int CHUNK_SIZE = 64;

typedef struct Chunk Chunk;

Chunk *Chunk_Init(ivec3s position);

void Chunk_Generate(Chunk *chunk);

void Chunk_Destroy(Chunk *chunk);

bool Chunk_IsReady(Chunk *chunk);

ivec3s Chunk_GetPosition(Chunk *chunk);

void Chunk_Render(Chunk *chunk);
