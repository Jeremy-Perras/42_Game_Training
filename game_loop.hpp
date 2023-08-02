#pragma once
namespace ve {
  class GameLoop {
  public:
    GameLoop();
    GameLoop(const GameLoop &src) = delete;
    GameLoop &operator=(const GameLoop &rhs) = delete;
    ~GameLoop();
    // Getter

  private:
  };
}  // namespace ve