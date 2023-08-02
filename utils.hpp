namespace ve {
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;
  static constexpr float HEIGHTVERTEX = 0.05F;
  static constexpr float WIDTHVERTEX = 0.05F;

  enum GameState { START, PLAYING, MENU, GAMELOOP };
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
    PLAYER,
    STAR,
    WHITE,
    PLAY,
    PAUSE,
    STOP,
    BACKGROUND,
    TEXTURESIZE,
    DISCARD,
  };

  constexpr int textureSize = 12;

}  // namespace ve