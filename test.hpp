
#include <GLUT/glut.h>
#include <OpenAL/alut.h>
#include <stdio.h>
#include <stdlib.h>

namespace ve {
  class test {
  public:
    test() {
      init();
      play();
    }
    ~test() {}

  private:
    void init(void);
    void play(void);
  };
}  // namespace ve