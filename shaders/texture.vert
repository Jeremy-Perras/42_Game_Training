#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(push_constant) uniform Push {
  vec4 color;
  vec4 offset;
  int index;
}
push;

void main() {
  vec2 position = vec2(inPosition.x + push.offset.x, inPosition.y + push.offset.y);
  gl_Position = vec4(position, 0.0, 1.0);
  fragTexCoord = inTexCoord;
}