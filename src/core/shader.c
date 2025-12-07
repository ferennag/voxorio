#include "shader.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include <GL/glew.h>
#include <assert.h>
#include <stdlib.h>

typedef struct Shader {
  GLuint id;
} Shader;

bool CompileShader(const char *path, GLenum type, GLuint *out) {
  void *contents = SDL_LoadFile(path, NULL);
  if (!contents) {
    return false;
  }

  GLuint shader = glCreateShader(type);
  const char *shaderSource = contents;
  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);
  SDL_free(contents);

  int status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    int length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    char *infoLog = calloc(length, sizeof(char));
    glGetShaderInfoLog(shader, length, NULL, infoLog);
    SDL_Log("Failed to compile %s: %s", path, infoLog);
    free(infoLog);
    glDeleteShader(shader);
    return false;
  }

  *out = shader;
  return true;
}

Shader *Shader_load(const char *vertexSource, const char *fragmentSource) {
  GLuint vertex, fragment;
  if (!CompileShader(vertexSource, GL_VERTEX_SHADER, &vertex)) {
    return NULL;
  }

  if (!CompileShader(fragmentSource, GL_FRAGMENT_SHADER, &fragment)) {
    glDeleteShader(vertex);
    return NULL;
  }

  GLuint program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  int status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    int length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    char *infoLog = calloc(length, sizeof(char));
    glGetProgramInfoLog(program, length, NULL, infoLog);
    SDL_Log("Failed to compile %s, %s: %s", vertexSource, fragmentSource, infoLog);
    free(infoLog);
    glDeleteProgram(program);
    return NULL;
  }

  Shader *shader = malloc(sizeof(Shader));
  shader->id = program;
  return shader;
}

void Shader_Destroy(Shader *shader) {
  assert(shader != NULL);
  glDeleteProgram(shader->id);
  free(shader);
}

void Shader_Bind(Shader *shader) {
  glUseProgram(shader->id);
}

void Shader_Unbind(Shader *shader) {
  glUseProgram(0);
}

void Shader_UniformVec3(Shader *shader, const char *name, const vec3s value) {
  GLint location = glGetUniformLocation(shader->id, name);
  if (location == -1) {
    SDL_Log("Can't find uniform location: %s", name);
    return;
  }

  glUniform3fv(location, 1, value.raw);
}

void Shader_UniformMat4(Shader *shader, const char *name, const mat4s value) {
  GLint location = glGetUniformLocation(shader->id, name);
  if (location == -1) {
    SDL_Log("Can't find uniform location: %s", name);
    return;
  }

  glUniformMatrix4fv(location, 1, GL_FALSE, &value.m00);
}
