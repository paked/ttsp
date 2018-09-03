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

// Macros to make interacting with the platform easier
#define mustLoadFromFile(fname, buf, sz) (assert(platform->loadFromFile(fname, buf, sz) == 0))
#define getWindowWidth() (platform->windowWidth)
#define getWindowHeight() (platform->windowHeight)
#define getDeltaTime() (platform->deltaTime)
#define keyJustDown(k) (platform->keyStateNow[k] && !platform->keyStateLast[k])
#define keyJustUp(k) (!platform->keyStateNow[k] && platform->keyStateLast[k])
#define keyDown(k) (platform->keyStateNow[k])
#define keyUp(k) (!platform->keyStateNow[k])

#include <physics.cpp>
#include <assets.cpp>
#include <draw.cpp>

#include <game_data.cpp>

int game_init(Platform* p) {
  platform = p;

  draw_init();

  draw.clear = vec3_black;

  return 0;
}

#define PLAYER_DRAG (0.80f)
#define PLAYER_ACCEL (30.0f)
#define PLAYER_MAX_SPEED (400.0f)
#define PLAYER_REACTIVITY (0.7f)
void game_update() {
  assert(platform != 0);

  assert(sizeof(GameData) < platform->permanentStorageSize);

  GameData* g = (GameData*) platform->permanentStorage;

  if (!platform->initialized) {
    // Initialize GameData in here...
    g->player.collider = aabb_init(getWindowWidth()/2 - 5, 500, 10, 20);
    g->ground = aabb_init(20, getWindowHeight() - 70, getWindowWidth() - 40, 60);

    platform->initialized = true;
  }

  {
    int direction = 0;
    if (keyDown(KEY_a)) {
      direction = -1.0f;
    } else if (keyDown(KEY_d)) {
      direction = 1.0f;
    } else {
      g->playerSpeed *= PLAYER_DRAG;
    }

    int sign = math_signi(g->playerSpeed);
    if (direction != 0) {
      real32 accel = PLAYER_ACCEL * direction;

      g->playerSpeed += accel;

      if (direction != sign) {
        g->playerSpeed += accel * PLAYER_REACTIVITY;
      }
    }

    if (fabs(g->playerSpeed) > PLAYER_MAX_SPEED) {
      g->playerSpeed = PLAYER_MAX_SPEED * sign;
    }

    actor_moveX(&g->player, g->playerSpeed * getDeltaTime(), g->ground);
  }

  {
    real32 amount = 300.0f * getDeltaTime();
    actor_moveY(&g->player, amount, g->ground);
  }

  // Render
  draw_begin();

  {
    Rect player = rect_init(g->player.collider.x, g->player.collider.y, g->player.collider.w, g->player.collider.h);
    Rect ground = rect_init(g->ground.x, g->ground.y, g->ground.w, g->ground.h);
    draw_rectangle(player, vec3_white);
    draw_rectangle(ground, vec3_white);
  }
}

void game_clean() {
  draw_clean();

  platform = 0;
}
