/*
 * This Too Shall Pass.
 *
 * A small platforming game written in a HMH style.
 *
 * By Harrison Shoebridge
 *
 */

#define HANDMADE_MATH_IMPLEMENTATION
#include <HandmadeMath.h>

#include <hmm_wrapper.hpp>

// Technically we don't really need this pointer, since we're still in the
// same compilation unit as the "runner" but it makes it more explicit that
// there is a CLEAN distinction between runner and game code.
Platform* platform = 0;

// Macros to make the platform easier
#define mustLoadFromFile(fname, buf, sz) (assert(platform->loadFromFile(fname, buf, sz) == 0))
#define getWindowWidth() (platform->windowWidth)
#define getWindowHeight() (platform->windowHeight)

#include <assets.cpp>
#include <draw.cpp>

int game_init(Platform* p) {
  platform = p;

  draw_init();

  draw.clear = vec3(200.0f, 50.0f, 100.0f);

  return 0;
}

void game_update() {
  assert(platform != 0);

  if (!platform->initialized) {
    platform->initialized = true;
  }

  draw_begin();

  Rect r = rect_init(0, 0, 200, 200);

  draw_rectangle(r, vec4(255.0f, 255.0f, 255.0f, 1.0f));
}

void game_clean() {
  draw_clean();

  platform = 0;
}
