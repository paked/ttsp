#include <SDL.h>
#include <stdio.h>

#define SIN_FREQ (220.0f)

SDL_AudioDeviceID deviceID = 0;
SDL_AudioSpec wavSpec = {0};

Uint32 wavLength;
Uint8* wavBuffer;
Uint8* wavHead;

int sampleBufferIndex = 0;
int sampleBufferSize = 0;
Sint16* sampleBuffer = 0;

void audioCallback(void* userdata, Uint8* stream, int len) {
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
  printf("len: %d\n", len);

  // Add sin wave to the mix

  len /= 2;
  Sint16* buf = (Sint16*) stream;

  for (int i = 0; i < len; i += 2) {
    Sint16 left = 0;
    {
      int index = sampleBufferIndex % sampleBufferSize;

      left = sampleBuffer[index];
      sampleBufferIndex += 1;
    }

    Sint16 right = 0;
    {
      int index = sampleBufferIndex % sampleBufferSize;

      right = sampleBuffer[index];
      sampleBufferIndex += 1;
    }

    buf[i] += left;
    buf[i + 1] += right;
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

  // Generate sine wave
  {
    int oneCycleLength = wavSpec.freq / SIN_FREQ;

    // 2 channels of samplesPerSecond length
    sampleBufferSize = 2 * oneCycleLength;

    sampleBuffer = (Sint16*) malloc(sampleBufferSize);

    float phase = 0;

    for (int i = 0; i < oneCycleLength; i++) {
      Sint16 v = 6000 * sinf((float) i / oneCycleLength * M_PI);

      sampleBuffer[i * 2] = v;
      sampleBuffer[i * 2 + 1] = v;

      phase += SIN_FREQ;
    }
  }

  SDL_PauseAudioDevice(deviceID, 0);

  // wait until we're don't playing
  while ( wavLength > 0 ) {
    SDL_Delay(100); 
  }

  return 0;
}
