#pragma once

#include "cglm/types-struct.h"

typedef struct World World;

World *World_Init(int seed);

void World_EnsureChunk(World *world, ivec3s position);

void World_Destroy(World *world);

void World_Update(World *world);

void World_Render(World *world, mat4s projection, mat4s view);
