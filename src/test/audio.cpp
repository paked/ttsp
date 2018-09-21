#include <SDL.h>
#include <stdio.h>

SDL_AudioDeviceID deviceID = 0;
SDL_AudioSpec wavSpec = {0};

Uint32 wavLength;
Uint8* wavBuffer;
Uint8* wavHead;

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
}

int main(int argc, char* argv[]) {
  if(SDL_Init(SDL_INIT_AUDIO) < 0) {
    printf("Could not init audio\n");

    return -1;
  }

  SDL_LoadWAV("data/sound/eerie.wav", &wavSpec, &wavBuffer, &wavLength);

  wavHead = wavBuffer;
  wavSpec.callback = audioCallback;

  deviceID = SDL_OpenAudioDevice(NULL, 0, &wavSpec, 0, 0);
  if (deviceID == 0) {
    printf("Failed to open audio: %s", SDL_GetError());

    return -1;
  }

  SDL_PauseAudioDevice(deviceID, 0);

  // wait until we're don't playing
  while ( wavLength > 0 ) {
    SDL_Delay(100); 
  }

  return 0;
}
