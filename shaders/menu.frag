#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;
layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D texSampler2;

layout(push_constant) uniform Push {
  int index;
} push;

void main() {
    if(push.index ==0){
    outColor = vec4(fragColor * texture(texSampler, fragTexCoord).rgb, 1.0);}
    else{outColor = vec4(fragColor * texture(texSampler2, fragTexCoord).rgb, 1.0);}
}