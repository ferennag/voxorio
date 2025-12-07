#pragma once

typedef struct Shader Shader;

Shader *Shader_load(const char *vertexSource, const char *fragmentSource);

void Shader_Destroy(Shader *shader);

void Shader_Bind(Shader *shader);

void Shader_Unbind(Shader *shader);
