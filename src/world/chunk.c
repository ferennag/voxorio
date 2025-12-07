#define FNL_IMPL
#include "chunk.h"
#include "SDL3/SDL_log.h"
#include "cglm/struct/affine-pre.h"
#include <FastNoiseLite.h>
#include <GL/glew.h>
#include <assert.h>
#include <cglm/struct/mat4.h>
#include <stddef.h>
#include <stdlib.h>

typedef enum CubeFace {
  CUBEFACE_FRONT,
  CUBEFACE_BACK,
  CUBEFACE_LEFT,
  CUBEFACE_RIGHT,
  CUBEFACE_TOP,
  CUBEFACE_BOTTOM,
} CubeFace;

typedef struct Vertex {
  vec3s position;
  vec2s uv;
  CubeFace face;
} Vertex;

typedef struct VertexArray {
  Vertex *data;
  size_t size;
  size_t capacity;
} VertexArray;

void VertexArray_Init(VertexArray *out) {
  out->size = 0;
  out->capacity = 100;
  out->data = calloc(out->capacity, sizeof(Vertex));
}

void VertexArray_Destroy(VertexArray *array) {
  free(array->data);
}

void VertexArray_Add(VertexArray *array, const Vertex vertex) {
  array->data[array->size] = vertex;
  array->size++;
  if (array->size >= array->capacity) {
    array->capacity *= 2;
    array->data = realloc(array->data, array->capacity * sizeof(Vertex));
  }
}

typedef struct Chunk {
  int seed;
  fnl_state noise;
  ivec3s position;
  bool ready;
  u64 *voxels;
  VertexArray vertices;

  GLuint vao, vbo;
} Chunk;

Chunk *Chunk_Init(int seed, ivec3s position) {
  Chunk *chunk = calloc(1, sizeof(Chunk));
  chunk->position = position;
  chunk->ready = false;
  chunk->seed = seed;

  fnl_state noise = fnlCreateState();
  noise.seed = chunk->seed;
  noise.noise_type = FNL_NOISE_CELLULAR;
  noise.frequency = 0.010f;
  noise.fractal_type = FNL_FRACTAL_FBM;
  chunk->noise = noise;

  VertexArray_Init(&chunk->vertices);
  SDL_Log("Creating chunk: (%d, %d, %d)", chunk->position.x, chunk->position.y, chunk->position.z);
  glCreateVertexArrays(1, &chunk->vao);
  glCreateBuffers(1, &chunk->vbo);

  return chunk;
}

void Chunk_Destroy(Chunk *chunk) {
  assert(chunk != NULL);
  SDL_Log("Destroying chunk: (%d, %d, %d)", chunk->position.x, chunk->position.y, chunk->position.z);

  glDeleteVertexArrays(1, &chunk->vao);
  glDeleteBuffers(1, &chunk->vbo);
  VertexArray_Destroy(&chunk->vertices);

  if (chunk->voxels) {
    free(chunk->voxels);
  }

  free(chunk);
}

void Chunk_AddFace(Chunk *chunk, int x, int y, int z, CubeFace face) {
  vec3s frontTopLeft = {{x - 0.5f, y + 0.5f, z + 0.5f}};
  vec3s frontTopRight = {{x + 0.5f, y + 0.5f, z + 0.5f}};
  vec3s frontBottomLeft = {{x - 0.5f, y - 0.5f, z + 0.5f}};
  vec3s frontBottomRight = {{x + 0.5f, y - 0.5f, z + 0.5f}};

  vec3s backTopLeft = {{x - 0.5f, y + 0.5f, z - 0.5f}};
  vec3s backTopRight = {{x + 0.5f, y + 0.5f, z - 0.5f}};
  vec3s backBottomLeft = {{x - 0.5f, y - 0.5f, z - 0.5f}};
  vec3s backBottomRight = {{x + 0.5f, y - 0.5f, z - 0.5f}};

  switch (face) {
    case CUBEFACE_FRONT: {
      VertexArray_Add(&chunk->vertices, (Vertex){frontTopLeft, {{0.0f, 1.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){frontBottomLeft, {{0.0f, 0.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){frontBottomRight, {{1.0f, 0.0f}}, face});

      VertexArray_Add(&chunk->vertices, (Vertex){frontTopLeft, {{0.0f, 1.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){frontBottomRight, {{1.0f, 0.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){frontTopRight, {{1.0f, 1.0f}}, face});
      break;
    }
    case CUBEFACE_BACK: {
      VertexArray_Add(&chunk->vertices, (Vertex){backBottomRight, {{0.0f, 0.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){backBottomLeft, {{1.0f, 0.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){backTopLeft, {{1.0f, 1.0f}}, face});

      VertexArray_Add(&chunk->vertices, (Vertex){backTopLeft, {{1.0f, 1.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){backTopRight, {{0.0f, 1.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){backBottomRight, {{0.0f, 0.0f}}, face});
      break;
    }
    case CUBEFACE_LEFT: {
      VertexArray_Add(&chunk->vertices, (Vertex){backBottomLeft, {{1.0f, 0.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){frontBottomLeft, {{0.0f, 0.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){backTopLeft, {{1.0f, 1.0f}}, face});

      VertexArray_Add(&chunk->vertices, (Vertex){backTopLeft, {{1.0f, 1.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){frontBottomLeft, {{0.0f, 0.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){frontTopLeft, {{0.0f, 1.0f}}, face});
      break;
    }
    case CUBEFACE_RIGHT: {
      VertexArray_Add(&chunk->vertices, (Vertex){frontTopRight, {{0.0f, 1.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){frontBottomRight, {{0.0f, 0.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){backBottomRight, {{1.0f, 0.0f}}, face});

      VertexArray_Add(&chunk->vertices, (Vertex){backBottomRight, {{1.0f, 0.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){backTopRight, {{1.0f, 1.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){frontTopRight, {{0.0f, 1.0f}}, face});
      break;
    }
    case CUBEFACE_TOP: {
      VertexArray_Add(&chunk->vertices, (Vertex){backTopLeft, {{0.0f, 1.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){frontTopLeft, {{0.0f, 0.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){frontTopRight, {{1.0f, 0.0f}}, face});

      VertexArray_Add(&chunk->vertices, (Vertex){frontTopRight, {{1.0f, 0.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){backTopRight, {{1.0f, 1.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){backTopLeft, {{0.0f, 1.0f}}, face});
      break;
    }
    case CUBEFACE_BOTTOM: {
      VertexArray_Add(&chunk->vertices, (Vertex){frontBottomLeft, {{1.0f, 0.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){backBottomLeft, {{1.0f, 1.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){backBottomRight, {{0.0f, 1.0f}}, face});

      VertexArray_Add(&chunk->vertices, (Vertex){backBottomRight, {{0.0f, 1.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){frontBottomRight, {{0.0f, 0.0f}}, face});
      VertexArray_Add(&chunk->vertices, (Vertex){frontBottomLeft, {{1.0f, 0.0f}}, face});
      break;
    }
  }
}

bool Chunk_IsVoxel(Chunk *chunk, int x, int y, int z) {
  return chunk->voxels[z * CHUNK_SIZE + x] & (((u64)1) << y);
}

bool Chunk_IsEmpty(Chunk *chunk, int x, int y, int z) {
  return (chunk->voxels[z * CHUNK_SIZE + x] & (((u64)1) << y)) == 0;
}

float Chunk_GetNoiseValue(Chunk *chunk, int x, int y, int z) {
  vec3s position = {
      {chunk->position.x * CHUNK_SIZE + x, chunk->position.y * CHUNK_SIZE, chunk->position.z * CHUNK_SIZE + z}};

  return fnlGetNoise3D(&chunk->noise, position.x, position.y, position.z);
}

void Chunk_Generate(Chunk *chunk) {
  if (chunk->ready) {
    return;
  }

  chunk->voxels = calloc(CHUNK_SIZE * CHUNK_SIZE, sizeof(u64));

  for (int z = 0; z < CHUNK_SIZE; ++z) {
    for (int x = 0; x < CHUNK_SIZE; ++x) {
      if (chunk->position.y > 0) {
        // We are in the air, for now we will just create empty chunks
        continue;
      } else if (chunk->position.y == 0) {
        // Ground level, we use the noise function to create the terrain
        float value = Chunk_GetNoiseValue(chunk, x, 0, z);
        int height = (int)floor(((value + 1.0f) / 2.0f * CHUNK_SIZE));
        chunk->voxels[z * CHUNK_SIZE + x] = (((u64)1) << height) - 1;
      } else {
        // Everything below is fully filled with voxels for now
        // TODO: Fill ground with noise based caves, etc
        chunk->voxels[z * CHUNK_SIZE + x] = (((u64)1) << (CHUNK_SIZE - 1)) - 1;
      }
    }
  }

  // TODO: Implement greedy meshing
  for (int z = 0; z < CHUNK_SIZE; ++z) {
    for (int y = 0; y < CHUNK_SIZE; ++y) {
      for (int x = 0; x < CHUNK_SIZE; ++x) {
        if (!Chunk_IsVoxel(chunk, x, y, z)) {
          continue;
        }

        if (z == 0 || Chunk_IsEmpty(chunk, x, y, z - 1)) {
          Chunk_AddFace(chunk, x, y, z, CUBEFACE_BACK);
        }

        if (z == CHUNK_SIZE - 1 || Chunk_IsEmpty(chunk, x, y, z + 1)) {
          Chunk_AddFace(chunk, x, y, z, CUBEFACE_FRONT);
        }

        if (x == 0 || Chunk_IsEmpty(chunk, x - 1, y, z)) {
          Chunk_AddFace(chunk, x, y, z, CUBEFACE_LEFT);
        }

        if (x == CHUNK_SIZE - 1 || Chunk_IsEmpty(chunk, x + 1, y, z)) {
          Chunk_AddFace(chunk, x, y, z, CUBEFACE_RIGHT);
        }

        if (y == CHUNK_SIZE - 1 || Chunk_IsEmpty(chunk, x, y + 1, z)) {
          Chunk_AddFace(chunk, x, y, z, CUBEFACE_TOP);
        }

        if (y == 0 || Chunk_IsEmpty(chunk, x, y - 1, z)) {
          Chunk_AddFace(chunk, x, y, z, CUBEFACE_BOTTOM);
        }
      }
    }
  }

  // TODO: I will want to run the chunk generation on a separate thread, so any OpenGL stuff must be removed from this
  // function!
  glBindVertexArray(chunk->vao);
  glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * chunk->vertices.size, chunk->vertices.data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv));
  glEnableVertexAttribArray(1);
  glVertexAttribIPointer(2, 1, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, face));
  glEnableVertexAttribArray(2);

  chunk->ready = true;
}

bool Chunk_IsReady(Chunk *chunk) {
  // TODO: After introducing multi-threaded chunk generation, we want mutexes in this function
  return chunk->ready;
}

ivec3s Chunk_GetPosition(Chunk *chunk) {
  return chunk->position;
}

void Chunk_Render(Chunk *chunk) {
  if (!chunk->ready) {
    return;
  }

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBindVertexArray(chunk->vao);
  glDrawArrays(GL_TRIANGLES, 0, chunk->vertices.size);
}

mat4s Chunk_ModelMat(Chunk *chunk) {
  mat4s model = glms_mat4_identity();
  return glms_translate(model, (vec3s){{
                                   chunk->position.x * CHUNK_SIZE,
                                   chunk->position.y * CHUNK_SIZE,
                                   chunk->position.z * CHUNK_SIZE,
                               }});
}
