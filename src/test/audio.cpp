#include <SDL.h>
#include <stdio.h>

SDL_AudioDeviceID deviceID = 0;
SDL_AudioSpec wavSpec = {0};

Uint32 wavLength;
Uint8* wavBuffer;
Uint8* wavHead;

int sinBufferIndex = 0;
int sinBufferSize = 0;
Sint16* sinBuffer = 0;

void audioCallback(void* userdata, Uint8* stream, int len) {
  /*
  if (wavLength == 0) {
    printf("Done!\n");

    memset(stream, 0, len);

    return;
  }

  printf("Going: %d!\n", wavLength);

  if ((Uint32) len > wavLength) {
    len = wavLength;
  }

  memcpy(stream, wavHead, len);

  wavHead += len;
  wavLength -= (Uint32) len;

  printf("wavLength: %d\n", wavLength);
  printf("len: %d\n", len);*/

  len /= 2;
  Sint16* buf = (Sint16*) stream;

  for (int i = 0; i < len; i += 2) {
    printf("audio?\n");
    Sint16 v = sinBuffer[sinBufferIndex % sinBufferSize];

    printf("%d\n", (int) v);

    buf[i] = v;
    buf[i + 1] = v;

    sinBufferIndex += 1;
  }
}

int main(int argc, char* argv[]) {
  if(SDL_Init(SDL_INIT_AUDIO) < 0) {
    printf("Could not init audio\n");

    return -1;
  }

  SDL_LoadWAV("data/sound/church.wav", &wavSpec, &wavBuffer, &wavLength);

  wavHead = wavBuffer;
  wavSpec.callback = audioCallback;

  deviceID = SDL_OpenAudioDevice(NULL, 0, &wavSpec, 0, 0);
  if (deviceID == 0) {
    printf("Failed to open audio: %s", SDL_GetError());

    return -1;
  }

  int samplesPerSecond = wavSpec.freq;

  sinBufferSize = 2 * samplesPerSecond;
  // `samplesPerSecond` for each channel (ie. 2)
  sinBuffer = (Sint16*) malloc(sinBufferSize);

  float phase = 0;
  float phaseIncrement = samplesPerSecond/400.0f;

  for (int i = 0; i < samplesPerSecond; i++) {
    Sint16 v = 6000 * sin(2.0f * 3.1415 * i * wavSpec.freq / 200);

    printf("%d\n", (int) v);

    sinBuffer[i*2] = v;
    sinBuffer[i*2 + 1] = v;

    phase += phaseIncrement;
  }

  SDL_PauseAudioDevice(deviceID, 0);

  // wait until we're don't playing
  while ( wavLength > 0 ) {
    SDL_Delay(100); 
  }

  return 0;
}
