#include "test.hpp"

#include <string>
//  function prototypes ---------------------------------------------
void init();
void display();
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);

// highly evil global variables ---------------------------------------------
#define NUM_BUFFERS 1
#define NUM_SOURCES 1
#define NUM_ENVIRONMENTS 1

ALfloat listenerPos[] = {0.0, 0.0, 4.0};
ALfloat listenerVel[] = {0.0, 0.0, 0.0};
ALfloat listenerOri[] = {0.0, 0.0, 1.0, 0.0, 1.0, 0.0};

ALfloat source0Pos[] = {-2.0, 0.0, 0.0};
ALfloat source0Vel[] = {0.0, 0.0, 0.0};

ALfloat source1Pos[] = {2.0, 0.0, 0.0};
ALfloat source1Vel[] = {0.0, 0.0, 0.0};

ALfloat source2Pos[] = {0.0, 0.0, -4.0};
ALfloat source2Vel[] = {0.0, 0.0, 0.0};

ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];

ALsizei size, freq;
ALenum format;
ALvoid* data;
int ch;

std::string getALErrorString(int err)

{
  switch (err)

  {
    case AL_INVALID_NAME:

      return "AL_INVALID_NAME";
    case AL_NO_ERROR:

      return "AL_NO_ERROR";

    case AL_INVALID_ENUM:

      return "AL_INVALID_ENUM";

    case AL_INVALID_VALUE:

      return "AL_INVALID_VALUE";

    case AL_INVALID_OPERATION:

      return "AL_INVALID_OPERATION";

    case AL_OUT_OF_MEMORY:

      return "AL_OUT_OF_MEMORY";

    default:
      return "No such error code";
  }
}

void ve::test::init(void) {
  alutInit(0, NULL);

  alListenerfv(AL_POSITION, listenerPos);
  alListenerfv(AL_VELOCITY, listenerVel);
  alListenerfv(AL_ORIENTATION, listenerOri);

  alGetError();  // clear any error messages

  if (alGetError() != AL_NO_ERROR) {
    printf("- Error creating buffers !!\n");
    exit(1);
  } else {
    printf("init() - No errors yet.");
  }

  // Generate buffers, or else no sound will happen!
  alGenBuffers(NUM_BUFFERS, buffer);

  alutLoadWAVFile("test2.wav", &format, &data, &size, &freq);
  alBufferData(buffer[0], format, data, size, freq);
  alutUnloadWAV(format, data, size, freq);

  alGetError(); /* clear error */
  alGenSources(NUM_SOURCES, source);

  if (alGetError() != AL_NO_ERROR) {
    printf("- Error creating sources !!\n");
    exit(2);
  } else {
    printf("init - no errors after alGenSources\n");
  }

  alSourcef(source[0], AL_PITCH, 1.0F);
  alSourcef(source[0], AL_GAIN, 1.0F);
  alSourcefv(source[0], AL_POSITION, source0Pos);
  alSourcefv(source[0], AL_VELOCITY, source0Vel);
  alSourcei(source[0], AL_BUFFER, buffer[0]);
  alSourcei(source[0], AL_LOOPING, AL_TRUE);
}

void ve::test::play() { alSourcePlay(source[0]); }