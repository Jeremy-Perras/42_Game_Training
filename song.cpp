#include "song.hpp"

#include <string>

// highly evil global variables ---------------------------------------------

void ve::Song::init(void) {
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

  alutLoadWAVFile(filename.data(), &format, &data, &size, &freq);
  alBufferData(buffer[0], format, data, size, freq);
  alutUnloadWAV(format, data, size, freq);

  alutLoadWAVFile(file.data(), &format, &data, &size, &freq);
  alBufferData(buffer[1], format, data, size, freq);
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
  alSourcei(source[0], AL_BUFFER, static_cast<int>(buffer[0]));
  alSourcei(source[0], AL_LOOPING, AL_TRUE);

  alSourcef(source[1], AL_PITCH, 1.0F);
  alSourcef(source[1], AL_GAIN, 1.0F);
  alSourcefv(source[1], AL_POSITION, source0Pos);
  alSourcefv(source[1], AL_VELOCITY, source0Vel);
  alSourcei(source[1], AL_BUFFER, static_cast<int>(buffer[1]));
  alSourcei(source[1], AL_LOOPING, AL_TRUE);
}

void ve::Song::playFile() { alSourcePlay(source[0]); }
void ve::Song::playMrRobot() { alSourcePlay(source[1]); }
void ve::Song::StopMrRobot() { alSourceStop(source[1]); }