#version 450

layout(binding = 0) uniform UniformBufferObject { float deltaTime; }
ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 fragColor;
layout(push_constant) uniform Push {
  vec4 color;
  vec2 offset;
  int index;
}
push;

void main() {
  vec2 position = vec2(inPosition.x + push.offset.x, inPosition.y + push.offset.y);
  gl_Position = vec4(position, 0.0, 1.0);
  fragTexCoord = inTexCoord;
  fragColor = inColor;
}