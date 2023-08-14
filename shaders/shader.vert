#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 fragColor;

layout(location = 0) out vec3 outColor;

layout(binding = 0) uniform Matrices { float i; }
matrices;

void main() {
  gl_Position = vec4(position, 0.0, 1.0);
  outColor = vec3(matrices.i, matrices.i, matrices.i);
}