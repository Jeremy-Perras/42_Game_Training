
#include <GLUT/glut.h>
#include <OpenAL/alut.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>

#define NUM_BUFFERS 2
#define NUM_SOURCES 2
#define NUM_ENVIRONMENTS 1

namespace ve {
  class Song {
  public:
    Song() { init(); }
    Song(const Song &src) = delete;
    Song &operator=(const Song &rhs) = delete;
    ~Song() {}
    void playFile(void);
    void playMrRobot(void);
    void StopMrRobot(void);

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

    std::string filename = "song/test2.wav";
    std::string file = "song/Mr.Robot.wav";
    void init(void);
  };
}  // namespace ve