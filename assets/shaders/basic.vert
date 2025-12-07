#version 460 core

layout (location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTextureCoords;
layout (location = 2) in int Face;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 TextureCoords;
layout(location = 2) out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

vec3 GetNormal(int face) {
  switch(face) {
    case 0:
      return vec3(0, 0, 1);
    case 1:
      return vec3(0, 0, -1);
    case 2:
      return vec3(-1, 0, 0);
    case 3:
      return vec3(1, 0, 0);
    case 4:
      return vec3(0, 1, 0);
    case 5:
      return vec3(0, -1, 0);
  }
}

void main() {
  gl_Position = projection * view * model * vec4(inPos, 1.0);
  FragPos= vec3(model * vec4(inPos, 1.0f));
  TextureCoords = inTextureCoords;
  Normal = mat3(transpose(inverse(model))) * GetNormal(Face);
}

