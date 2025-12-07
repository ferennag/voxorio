#pragma once

#include "SDL3/SDL_events.h"
#include "cglm/types-struct.h"

typedef struct Camera Camera;

Camera *Camera_Init(const vec3s position);

void Camera_Destroy(Camera *camera);

void Camera_LookAt(Camera *camera, const vec3s target);

mat4s Camera_ViewMat(Camera *camera);

vec3s Camera_GetPosition(Camera *camera);

void Camera_HandleMouseMotion(Camera *camera, SDL_MouseMotionEvent *event);

void Camera_HandleKeyboardEvent(Camera *camera, SDL_KeyboardEvent *event);

void Camera_Update(Camera *camera);
