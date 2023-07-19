#version 450


layout(location= 0) in vec2 position;
layout(location = 1) in vec3 fragColor;

layout(location = 0) out vec3 outColor;

// layout(push_constant)uniform Push {
//   vec3 color;
// } push;
layout(set = 0, binding = 0) uniform GlobalUbo {
    float deltaTime;
} ubo;

void main() {
  
  gl_Position = vec4( position , 0.0, 1.0);
  outColor = vec3(sin(ubo.deltaTime),1,1);
}