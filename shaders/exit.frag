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
    if (push.index == 27) {
    discard;
  } 
    if (discardAlpha(texture(tex, fragTexCoord).r) && discardAlpha(texture(tex, fragTexCoord).g)
        && discardAlpha(texture(tex, fragTexCoord).b)) {
      outColor = push.color;
    } else {
      switch (push.index) {
        case 6:
          if (texelFetch(tex, ivec2(fragTexCoord * vec2(512.0)), 0).r > 0.5) {
            outColor
                = vec4(texelFetch(tex, ivec2(fragTexCoord * vec2(500.0)), 0).rgb, push.color.a);
          } else {
            outColor = vec4(push.color.rgba);
          }
          break;
        case 7:
          if (texelFetch(tex, ivec2(fragTexCoord * vec2(512.0)), 0).g > 0.5) {
            outColor
                = vec4(texelFetch(tex, ivec2(fragTexCoord * vec2(500.0)), 0).rgb, push.color.a);
          } else {
            outColor = vec4(push.color.rgba);
          }
          break;
        case 8:
          if (texelFetch(tex, ivec2(fragTexCoord * vec2(512.0)), 0).b > 0.5) {
            outColor
                = vec4(texelFetch(tex, ivec2(fragTexCoord * vec2(500.0)), 0).rgb, push.color.a);
          } else {
            outColor = vec4(push.color.rgba);
          }
          break;
        case 9:
          outColor = vec4(texture(tex, fragTexCoord).rgb, push.color.a);
          break;
        case 10:
          outColor = vec4(texture(tex, fragTexCoord).rgb, push.color.a);
          break;
        case 11:
          outColor = vec4(texture(tex, fragTexCoord).rgb, push.color.a);
          break;
        case 23:
          outColor = vec4(texelFetch(tex, ivec2(fragTexCoord), 0).rgb, push.color.a);
          outColor.b = texture(tex, fragTexCoord).b;
          break;
        default:
          outColor = vec4(texture(tex, fragTexCoord).rgb * push.color.rgb, push.color.a);
          break;
      }
    }
    
    if (texture(tex, fragTexCoord).a < 0.3 || push.color.w < 0.3) {
      discard;
    }
  
}