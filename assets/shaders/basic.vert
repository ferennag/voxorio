#version 460 core

layout (location = 0) in vec3 inPos;

uniform mat4 projection;
uniform mat4 view;

void main() {
  gl_Position = projection * view * vec4(inPos, 1.0);
}

