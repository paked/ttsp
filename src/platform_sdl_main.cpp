// External libraries
#include <SDL.h>
#include <glad/glad.h>

// Internal libraries
#include <platform_common.hpp>
#include <config.hpp>

// Game code
#include <game.cpp>

//
// Platform interface
//
#define ALLOCATE_MEMORY_FUNC malloc
#define FREE_MEMORY_FUNC free

SDL_Window *window;
SDL_GLContext glContext;

Platform realPlatform = {0};

char* dataPath = 0;

uint32 keySDLToPlatform(SDL_KeyboardEvent event) {
  uint32 key = KEY_unknown;
  uint32 scancode = event.keysym.scancode;

  if (scancode == SDL_SCANCODE_LSHIFT) {
    key = KEY_shift;
  } else if (scancode == SDL_SCANCODE_GRAVE) {
    key = KEY_grave;
  } else if (scancode == SDL_SCANCODE_LCTRL) {
    key = KEY_ctrl;
  } else if (scancode == SDL_SCANCODE_TAB) {
    key = KEY_tab;
  } else if (scancode == SDL_SCANCODE_SPACE) {
    key = KEY_space;
  } else if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z) {
    key = KEY_a + (scancode - SDL_SCANCODE_A);
  } else if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_0) {
    key = KEY_1 + (scancode - SDL_SCANCODE_1);
  }

  return key;
}

int sdl_loadFromFile(char *filename, void **buffer, MemoryIndex* size) {
  char fname[MAX_PATH] = {0};

  strcat(fname, dataPath);
  strcat(fname, filename);

  SDL_RWops *rw = SDL_RWFromFile(fname, "rb");

  if (rw == 0) {
    return 1;
  }

  *size = SDL_RWsize(rw);

  // TODO(harrison): load into an asset specific file arena
  char* buf = (char*) ALLOCATE_MEMORY_FUNC(*size + 1);

  MemoryIndex readTotal = 0;
  MemoryIndex readLast = 1;

  char* read = buf;

  while (readTotal < *size && readLast != 0) {
    readLast = SDL_RWread(rw, read, 1, (*size - readTotal));
    readTotal += readLast;

    read += readLast; // advance memory buffer
  }

  SDL_RWclose(rw);

  if (readTotal != *size) {
    FREE_MEMORY_FUNC(buf);

    logln("ERROR: Unable to read file...");

    return 1;
  }

  buf[*size] = 0;

  *buffer = buf;

  return 0;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    logln("Usage: ./game $DATA_PATH. Refer to provided platform specific run-script. $DATA_PATH must end in a slash.");

    return 1;
  }

  dataPath = argv[1];

  // HACK(harrison): make sure that provided path is a "directory", and in
  // order to make future operations simpler ensure that it ends in a slash.
  if (dataPath[strlen(dataPath) - 1] != '/') {
    logln("ERROR: $DATA_PATH must end in a slash. Are you sure you're running through the run-script?");

    return 1;
  }

  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    logln("ERROR: Could not init SDL video");

    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  window = SDL_CreateWindow(
      WINDOW_NAME,
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      WINDOW_WIDTH,
      WINDOW_HEIGHT,
      SDL_WINDOW_OPENGL);

  if (!window) {
    logfln("ERROR: Could not create SDL window: %s", SDL_GetError());

    return 1;
  }

  glContext = SDL_GL_CreateContext(window);

  if (!glContext) {
    logfln("ERROR: Could not create OpenGL context: %s", SDL_GetError());

    return 1;
  }

  if (SDL_GL_SetSwapInterval(1) < 0) {
    logfln("ERROR: could not set SDL swap interval: %s", SDL_GetError());

    // TODO(harrison): create an internal system for handling framerate if
    // vsync doesn't exist
  }

  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    logln("ERROR: Could not load glad");

    return 1;
  }

  realPlatform.permanentStorageSize = PERMANENT_MEMORY_SIZE;
  realPlatform.permanentStorage = ALLOCATE_MEMORY_FUNC(realPlatform.permanentStorageSize);

  realPlatform.transientStorageSize = TRANSIENT_MEMORY_SIZE;
  realPlatform.transientStorage = ALLOCATE_MEMORY_FUNC(realPlatform.transientStorageSize);

  // NOTE(harrison): Our allocator does not zero out our memory for us, so we need to do it ourself
  // TODO(harrison): figure out if this is prevalent enough for us to write a 'custom allocator'
  memset(realPlatform.permanentStorage, 0, realPlatform.permanentStorageSize);
  memset(realPlatform.transientStorage, 0, realPlatform.transientStorageSize);

  realPlatform.loadFromFile = sdl_loadFromFile;
  realPlatform.windowWidth = WINDOW_WIDTH;
  realPlatform.windowHeight = WINDOW_HEIGHT;

  if (game_init(&realPlatform) != 0) {
    logln("ERROR: could not init game!");

    return 1;
  }

  uint64 perfCounterFreq = SDL_GetPerformanceFrequency();
  SDL_Event event;
  while (!realPlatform.quit) {
    realPlatform.time = SDL_GetTicks();

    // Copy "now" key state into the last key state buffer, and reset the new key state
    for (uint32 i = 0; i < MAX_KEY; i++) {
      realPlatform.keyStateLast[i] = realPlatform.keyStateNow[i];
    }

    uint64 timeFrameStart = SDL_GetPerformanceCounter();
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          {
            realPlatform.quit = true;
          } break;
        case SDL_WINDOWEVENT:
          {
            switch (event.window.event) {
              case SDL_WINDOWEVENT_SIZE_CHANGED:
                {
                  realPlatform.windowWidth = event.window.data1;
                  realPlatform.windowHeight = event.window.data2;
                } break;
            }

          } break;
        case SDL_KEYUP:
        case SDL_KEYDOWN:
          {
            uint32 key = keySDLToPlatform(event.key);

            if (key == KEY_unknown) {
              logln("Could not identify key!");

              break;
            }

            realPlatform.keyStateNow[key] = event.key.state == SDL_PRESSED;
          } break;
      }
    }

    game_update();

    SDL_GL_SwapWindow(window);

    uint64 timeFrameEnd = SDL_GetPerformanceCounter();
    uint64 timeElapsed = timeFrameEnd - timeFrameStart;

    real64 deltaTime = ((((real64)timeElapsed) / (real64)perfCounterFreq));
    // real64 fps = (real64)perfCounterFreq / (real64)timeElapsed;

    // printf("%f ms/f, %.02ff/s\n", deltaTime, fps);

    realPlatform.deltaTime = deltaTime;
  }

  game_clean();

  return 0;
}
