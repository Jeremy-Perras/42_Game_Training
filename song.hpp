
#include <GLUT/glut.h>
#include <OpenAL/alut.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>

#define NUM_BUFFERS 1
#define NUM_SOURCES 1
#define NUM_ENVIRONMENTS 1

namespace ve {
  class Song {
  public:
    Song() {
      init();
      play();
    }
    Song(const Song &src) = delete;
    Song &operator=(const Song &rhs) = delete;
    ~Song() {}

  private:
    ALfloat listenerPos[3] = {0.0, 0.0, 4.0};
    ALfloat listenerVel[3] = {0.0, 0.0, 0.0};
    ALfloat listenerOri[6] = {0.0, 0.0, 1.0, 0.0, 1.0, 0.0};

    ALfloat source0Pos[3] = {-2.0, 0.0, 0.0};
    ALfloat source0Vel[3] = {0.0, 0.0, 0.0};

    ALuint buffer[NUM_BUFFERS];
    ALuint source[NUM_SOURCES];
    ALuint environment[NUM_ENVIRONMENTS];

    ALsizei size, freq;
    ALenum format;
    ALvoid *data;

    std::string filename = "test2.wav";
    void init(void);
    void play(void);
  };
}  // namespace ve