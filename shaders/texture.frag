#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;
layout(binding = 0) uniform sampler2D texSampler[12];

layout(push_constant) uniform Push {
  vec4 color;
  vec4 offset;
  int index;
}
push;

void discardAlpha(float w) {
  if (outColor.w < .8) {
    discard;
  }
}

void main() {
  switch (push.index) {
    case 0:
      outColor = vec4(push.color.rgb * texture(texSampler[0], fragTexCoord).rgb, push.color.a);
      break;
    case 1:
      outColor = vec4(push.color.rgb * texture(texSampler[1], fragTexCoord).rgb, push.color.a);
      break;
    case 2:
      outColor = vec4(push.color.rgb * texture(texSampler[2], fragTexCoord).rgb, push.color.a);
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
    else 
      outColor = push.color;
      break;
    case 7:
      outColor = vec4(push.color.rgb * texture(texSampler[7], fragTexCoord).rgb, push.color.a);
      break;
    case 8:
      outColor = vec4(push.color.rgb * texture(texSampler[8], fragTexCoord).rgb, push.color.a);
      break;
    case 9:
      outColor = push.color;
      break;
    case 10:
      outColor = push.color;
      break;
    case 11:
      outColor = push.color;
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
      outColor = push.color;
      break;
    case 15:
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
}
