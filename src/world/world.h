#pragma once

#include "cglm/types-struct.h"
#include "core/camera.h"

typedef struct World World;

World *World_Init(int seed);

void World_EnsureChunk(World *world, ivec3s position);

void World_Destroy(World *world);

void World_Update(World *world);

void World_Render(World *world, mat4s projection, Camera *camera);
