#version 460 core

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TextureCoords;
layout(location = 2) in vec3 Normal;

out vec4 FragColor;

uniform sampler2D sampler;
uniform vec3 sunDirection;
uniform vec3 sunColor;
uniform vec3 eye;

void main() {
  vec3 normal = normalize(Normal);
  vec3 lightDir = normalize(-sunDirection);
  float ambientStrength = 0.1f;
  vec3 ambient = ambientStrength * sunColor;

  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = diff * sunColor;

  float specularStrength = 0.5f;
  vec3 viewDir = normalize(eye - FragPos);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * sunColor;

  FragColor = vec4((ambient + diffuse + specular), 1.0) * texture(sampler, TextureCoords);
}
