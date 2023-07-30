#version 450

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;
layout(binding = 1) uniform sampler2D texSampler[10];



layout(push_constant) uniform Push {
  int index;
} push;

void main() {
  switch(push.index) {
    case 0:
   outColor = vec4( texture(texSampler[0], fragTexCoord).rgb, 0.1);
    break;
  case 1:
  outColor = vec4( texture(texSampler[1], fragTexCoord).rgb, 1.0);
    break;
  case 2:
   outColor = vec4( texture(texSampler[2], fragTexCoord).rgb, 0.1);
    break;
     case 3:
   outColor = vec4( texture(texSampler[3], fragTexCoord).rgb, 0.1);
    break;
     case 4:
   outColor = vec4( texture(texSampler[4], fragTexCoord).rgb, 0.1);
    break;
     case 5:
   outColor = vec4( texture(texSampler[5], fragTexCoord).rgb, 0.1);
    break;
     case 6:
   outColor = vec4( texture(texSampler[6], fragTexCoord).rgb, 0.1);
    break;
     case 7:
   outColor = vec4( texture(texSampler[7], fragTexCoord).rgb, 0.1);
    break;
 case 8:
   outColor = vec4( texture(texSampler[8], fragTexCoord).rgb, 0.1);
    break;
     case 9:
   outColor = vec4( texture(texSampler[9], fragTexCoord).rgba);
    if (outColor.w < 0.8) {
        discard;
    }
    break;
     case 100:
   outColor = vec4( 1.0,0.0,0.0,1);
    break;
     case 101:
  outColor = vec4( 0.0,1.0,0.0,1);
    break;
     case 102:
  outColor = vec4( 0.0,0.0,1.0,1);
    break;
  default:
  outColor = vec4( 1.0,1.0,1.0,0.1);

}
}