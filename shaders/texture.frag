#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;
layout (location = 1) flat in int inTexIndex;
layout (set = 0, binding = 1) uniform sampler2D texSampler[];



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
  switch (push.index) {
    case 0:
      outColor = vec4(push.color.rgb * texture(texSampler[nonuniformEXT(inTexIndex)], fragTexCoord).rgb, push.color.a);
      break;
    case 1:
      outColor = vec4(push.color.rgb * texture(texSampler[nonuniformEXT(inTexIndex)], fragTexCoord).rgb, push.color.a);
      break;
    case 2:
      outColor = vec4(push.color.rgb * texture(texSampler[nonuniformEXT(inTexIndex)], fragTexCoord).rgb, push.color.a);
      break;
    case 3:
      outColor = vec4(push.color.rgb * texture(texSampler[3], fragTexCoord).rgb, push.color.a);
      break;
    case 4:
      outColor = vec4(push.color.rgb * texture(texSampler[4], fragTexCoord).rgb, push.color.a);
      break;
    case 5:
      outColor = vec4(push.color.rgb * texture(texSampler[5], fragTexCoord).rgb, push.color.a);
      break;
    case 6:
    
    if(texture(texSampler[6], fragTexCoord).rgb !=vec3(1.0,1.0,1.0)){
     outColor= vec4(texture(texSampler[6], fragTexCoord).rgb, push.color.a); 
    }
    else {
      outColor = push.color;}
    discardAlpha(outColor.w);
      break;
    case 7:
      outColor = vec4(push.color.rgb * texture(texSampler[7], fragTexCoord).rgb, push.color.a);
      break;
    case 8:
      outColor = vec4(push.color.rgb * texture(texSampler[8], fragTexCoord).rgb, push.color.a);
      break;
    case 9:
     outColor = vec4(push.color.rgb * texture(texSampler[12], fragTexCoord).rgb, push.color.a);
      break;
    case 10:
     outColor = vec4(push.color.rgb * texture(texSampler[13], fragTexCoord).rgb, push.color.a);
      break;
    case 11:
     outColor = vec4(push.color.rgb * texture(texSampler[14], fragTexCoord).rgb, push.color.a);
      break;
    case 12:
      outColor = vec4(push.color * texture(texSampler[9], fragTexCoord).rgba);
      discardAlpha(outColor.w);
      break;
    case 13:
      outColor = vec4(texture(texSampler[11], fragTexCoord).rgba);
      discardAlpha(outColor.w);
      break;
    case 14:
     outColor = vec4(push.color.rgb * texture(texSampler[15], fragTexCoord).rgb, push.color.a);
     
      break;
    case 15:
    outColor = vec4(push.color.rgb * texture(texSampler[16], fragTexCoord).rgb, push.color.a); 
      outColor = push.color;

      break;
    case 16:
     
  outColor = push.color; 
      break;
    case 17:
      outColor = push.color;
      break;
    case 20:
      discard;
      break;
    case 21:
      discard;
      break;
    default:
      outColor = vec4(push.color.rgb * texture(texSampler[10], fragTexCoord).rgb, push.color.a);
  }
  discardAlpha(outColor.w);
}
