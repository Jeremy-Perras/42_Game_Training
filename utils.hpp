#pragma once
namespace ve {
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;
  static constexpr float HEIGHTVERTEX = 0.05F;
  static constexpr float WIDTHVERTEX = 0.05F;
  static constexpr float DRAW_NUM_PARTICLES = 80128;
  static constexpr float DRAW_NUM_STARS = 75000;
  static constexpr float DRAW_PARTICLE_WORK_GROUP_SIZE = 256;

  enum GameState { START, PLAYING, MENU, GAMELOOP, SBYS, WAIT, TEST };
  struct playerCoordinate {
    float x;
    float y;
    float Angle;
  };

  enum TextureIndex {
    ARROWLEFT,
    ARROWUP,
    ARROWRIGHT,
    F0,
    F1,
    F2,
    BRUSHRED,
    BRUSHGREEN,
    BRUSHBLUE,
    RED,
    GREEN,
    BLUE,
    PLAYERUP,
    STAR,
    WHITE,
    PLAY,
    STEPBYSTEP,
    STOP,
    BACKGROUND,
    PLAYERDOWN,
    PLAYERRIGHT,
    PLAYERLEFT,
    BACKGROUNDSTART,
    STARTBUTTON,
    TEXTURESIZE,
    DISCARD,
    DONOTSHOW,
  };

  constexpr int textureSize = 16;
  // constexpr int textureSize2 = 2;
}  // namespace ve