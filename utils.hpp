namespace ve {
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
  // constexpr int textureSize = TextureIndex::TEXTURESIZE;
  constexpr int textureSize = 12;

}  // namespace ve