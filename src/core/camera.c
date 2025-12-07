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

  float yaw, pitch;
} Camera;

Camera *Camera_Init(const vec3s position) {
  Camera *camera = calloc(1, sizeof(Camera));
  camera->position = position;
  camera->up = (vec3s){{0.0f, 1.0f, 0.0f}};
  camera->yaw = -90;

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
  float speed = 0.1f;
  camera->yaw += event->xrel * speed;
  camera->pitch -= event->yrel * speed;

  if (camera->pitch > 89.0f) {
    camera->pitch = 89.0f;
  }

  if (camera->pitch < -89.0f) {
    camera->pitch = -89.0f;
  }

  vec3s direction;
  direction.x = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
  direction.y = sin(glm_rad(camera->pitch));
  direction.z = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
  camera->front = glms_normalize(direction);
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

  if (camera->keyboardState[MOVEMENT_FORWARD]) {
    camera->position = glms_vec3_add(camera->position, glms_vec3_scale(camera->front, speed));
  }

  if (camera->keyboardState[MOVEMENT_BACK]) {
    camera->position = glms_vec3_sub(camera->position, glms_vec3_scale(camera->front, speed));
  }

  if (camera->keyboardState[MOVEMENT_LEFT]) {
    vec3s left = glms_normalize(glms_cross(camera->front, camera->up));
    camera->position = glms_vec3_sub(camera->position, glms_vec3_scale(left, speed));
  }

  if (camera->keyboardState[MOVEMENT_RIGHT]) {
    vec3s left = glms_normalize(glms_cross(camera->front, camera->up));
    camera->position = glms_vec3_add(camera->position, glms_vec3_scale(left, speed));
  }

  if (camera->keyboardState[MOVEMENT_JUMP]) {
    camera->position = glms_vec3_add(camera->position, glms_vec3_scale(camera->up, speed));
  }

  if (camera->keyboardState[MOVEMENT_CROUCH]) {
    camera->position = glms_vec3_sub(camera->position, glms_vec3_scale(camera->up, speed));
  }
}
