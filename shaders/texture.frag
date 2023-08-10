#version 450


layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;
layout (set = 0, binding = 0) uniform sampler2D tex;

// layout (set = 0, binding = 3) uniform sampler2D texSampler[];



layout(push_constant) uniform Push {
  vec4 color;
  vec4 offset;
  int index;
}
push;

void discardAlpha(float w) {
  if (outColor.w ==0.0) {
    discard;
  }
}

void main() {
  
    outColor = vec4(push.color.rgb * texture(tex, fragTexCoord).rgb, push.color.a);
   
  if(texture(tex, fragTexCoord).a<0.3){
    discard;
  }
 
}
