#version 450


layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;
layout(set = 0, binding = 0) uniform sampler2D tex;

layout(push_constant) uniform Push {
  vec4 color;
  vec4 offset;
  int index;
}
push;

bool discardAlpha(float w) {
  if (w <= 1.0 && w >= 0.8) {
    return true;
  }
  return false;
}

void main() {
   
  outColor = vec4(texture(tex, fragTexCoord).rgb , 1.0);
  
}