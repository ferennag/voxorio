#include "camera.h"
#include "SDL3/SDL_scancode.h"
#include "cglm/struct/vec3.h"
#include "cglm/types-struct.h"
#include <assert.h>
#include <cglm/struct/cam.h>
#include <stdlib.h>

typedef enum MovementKeys {
  MOVEMENT_FORWARD,
  MOVEMENT_BACK,
  MOVEMENT_LEFT,
  MOVEMENT_RIGHT,
  MOVEMENT_JUMP,
  MOVEMENT_CROUCH,
  MOVEMENT_ALL,
} MovementKeys;

typedef struct Camera {
  bool keyboardState[MOVEMENT_ALL];
  vec3s position;
  vec3s front;
  vec3s up;
} Camera;

Camera *Camera_Init(const vec3s position) {
  Camera *camera = calloc(1, sizeof(Camera));
  camera->position = position;
  camera->up = (vec3s){{0.0f, 1.0f, 0.0f}};

  vec3s target = glms_vec3_sub(camera->position, (vec3s){{0.0f, 0.0f, 1.0f}});
  Camera_LookAt(camera, target);
  return camera;
}

void Camera_Destroy(Camera *camera) {
  assert(camera != NULL);
  free(camera);
}

void Camera_LookAt(Camera *camera, const vec3s target) {
  camera->front = glms_vec3_sub(target, camera->position);
  glms_vec3_normalize(camera->front);
}

mat4s Camera_ViewMat(Camera *camera) {
  vec3s target = glms_vec3_add(camera->position, camera->front);
  return glms_lookat(camera->position, target, camera->up);
}

void Camera_HandleMouseMotion(Camera *camera, SDL_MouseMotionEvent *event) {
}

void Camera_HandleKeyboardEvent(Camera *camera, SDL_KeyboardEvent *event) {
  switch (event->key) {
    case SDLK_SPACE: {
      camera->keyboardState[MOVEMENT_JUMP] = event->down;
      break;
    }
    case SDLK_W: {
      camera->keyboardState[MOVEMENT_FORWARD] = event->down;
      break;
    }
    case SDLK_S: {
      camera->keyboardState[MOVEMENT_BACK] = event->down;
      break;
    }
    case SDLK_A: {
      camera->keyboardState[MOVEMENT_LEFT] = event->down;
      break;
    }
    case SDLK_D: {
      camera->keyboardState[MOVEMENT_RIGHT] = event->down;
      break;
    }
  }

  camera->keyboardState[MOVEMENT_CROUCH] = event->mod & SDL_SCANCODE_LCTRL;
}

void Camera_Update(Camera *camera) {
  float speed = 0.2f;
  vec3s velocity;

  if (camera->keyboardState[MOVEMENT_FORWARD]) {
    velocity.z = -speed;
  }

  if (camera->keyboardState[MOVEMENT_BACK]) {
    velocity.z = speed;
  }

  if (camera->keyboardState[MOVEMENT_LEFT]) {
    velocity.x = -speed;
  }

  if (camera->keyboardState[MOVEMENT_RIGHT]) {
    velocity.x = speed;
  }

  if (camera->keyboardState[MOVEMENT_JUMP]) {
    velocity.y = speed;
  }

  if (camera->keyboardState[MOVEMENT_CROUCH]) {
    velocity.y = -speed;
  }

  camera->position = glms_vec3_add(camera->position, velocity);
}
