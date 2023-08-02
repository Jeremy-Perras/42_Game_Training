#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragColor;
layout(location = 0) out vec4 outColor;
layout(binding = 1) uniform sampler2D texSampler[12];

layout(push_constant) uniform Push {
  vec4 color;
  vec2 offset;
  int index;
}
push;

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
      outColor = vec4(push.color.rgb * texture(texSampler[6], fragTexCoord).rgb, push.color.a);
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
      if (outColor.w < 0.8) {
        discard;
      }
      break;
    case 13:
      outColor = vec4(texture(texSampler[11], fragTexCoord).rgba);
      if (outColor.w < 0.8) {
        discard;
      }
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

    default:
      outColor = vec4(push.color.rgb * texture(texSampler[10], fragTexCoord).rgb, push.color.a);
  }
}