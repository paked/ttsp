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
#define getCurrentFPS() (platform->currentFPS)
#define getWindowWidth() (platform->windowWidth)
#define getWindowHeight() (platform->windowHeight)
#define getDeltaTime() (platform->deltaTime)
#define keyJustDown(k) (platform->keyStateNow[k] && !platform->keyStateLast[k])
#define keyJustUp(k) (!platform->keyStateNow[k] && platform->keyStateLast[k])
#define keyDown(k) (platform->keyStateNow[k])
#define keyUp(k) (!platform->keyStateNow[k])

#define GAME_VIRTUAL_WIDTH (320)
#define GAME_VIRTUAL_HEIGHT (184)

#include <memory_arena.hpp>
#include <physics.cpp>
#include <assets.cpp>
#include <draw.cpp>
#include <mixer.cpp>

#include <level.cpp>
#include <game_data.cpp>

Sound exampleSound = {0};
Sound exampleSound2 = {0};

int game_init(Platform* p) {
  platform = p;

  draw_init();

  draw.clear = vec3_black;

  exampleSound = sound_load("sound/church.wav");
  exampleSound2 = sound_load("sound/bloop_00.wav");

  return 0;
}

#define PLAYER_DRAG (0.50f)
#define PLAYER_ACCEL (24.0f)
#define PLAYER_MAX_SPEED_X (60.0f)
#define PLAYER_MAX_SPEED_Y (300.0f)
#define PLAYER_REACTIVITY (0.7f)

#define PLAYER_GRAVITY (20.0f)
#define PLAYER_JUMP_ACCEL_DRAG (0.86f)
#define PLAYER_JUMP_ACCEL (-80.0f)
void game_update() {
  assert(platform != 0);

  assert(sizeof(GameData) < platform->permanentStorageSize);

  GameData* g = (GameData*) platform->permanentStorage;

  if (!platform->initialized) {
    // Initialize GameData in here...
    g->player.collider = aabb_init(0, 0, 8, 12);

    mixer_init();

    mixer_playSound(&exampleSound, false, 1.0f);
    mixer_playSound(&exampleSound2, false, 1.0f);

    memoryArena_init(&g->memoryArena,
        platform->permanentStorageSize - sizeof(GameData),
        (uint8*) platform->permanentStorage + sizeof(GameData));

    level_init(&g->level, &g->memoryArena);
    level_load(&g->level);

    platform->initialized = true;
  }

  {
    int direction = 0;
    if (keyDown(KEY_a)) {
      direction = -1.0f;
    } else if (keyDown(KEY_d)) {
      direction = 1.0f;
    } else {
      g->playerSpeed.x *= PLAYER_DRAG;
    }

    int sign = math_signi(g->playerSpeed.x);
    if (direction != 0) {
      real32 accel = PLAYER_ACCEL * direction;

      g->playerSpeed.x += accel;

      if (direction != sign) {
        g->playerSpeed.x += accel * PLAYER_REACTIVITY;
      }
    }

    if (fabs(g->playerSpeed.x) > PLAYER_MAX_SPEED_X) {
      g->playerSpeed.x = PLAYER_MAX_SPEED_X * sign;
    }

    actor_moveX(&g->player, g->playerSpeed.x * getDeltaTime(), &g->level);
  }

  {
    g->playerSpeed.y += PLAYER_GRAVITY;

    {
      g->jumpAccel *= PLAYER_JUMP_ACCEL_DRAG;

      if (keyDown(KEY_space)) {
        g->playerSpeed.y += g->jumpAccel;
      }
    }

    if (fabs(g->playerSpeed.y) > PLAYER_MAX_SPEED_Y) {
    int sign = math_signi(g->playerSpeed.y);
      g->playerSpeed.y = PLAYER_MAX_SPEED_Y * sign;
    }

    actor_moveY(&g->player, g->playerSpeed.y * getDeltaTime(), &g->level);

    if (g->player.onGround && keyJustDown(KEY_space)) {
      g->jumpAccel = PLAYER_JUMP_ACCEL;

      g->playerSpeed.y = 0;
    }
  }

  g->player.onGround = false;

  // Render
  draw_virtual_begin();
  {

    Rect player = rect_init(g->player.collider.x, g->player.collider.y, g->player.collider.w, g->player.collider.h);
    draw_rectangle(player, vec3_white);

    Rect tile = rect_init(0, 0, 0, 0);

    for (MemoryBlock* block = g->level.arena->first; block != 0; block = block->next) {
      LevelChunk* chunk = (LevelChunk*) block->start;
      for (int y = 0; y < chunk->height; y++) {
        for (int x = 0; x < chunk->width; x++) {
          int* c = chunk->data + y * chunk->width + x;

          if (*c < 0) {
            continue;
          }

          tile.x = (chunk->i * CHUNK_WIDTH + x) * 8;
          tile.y = y * 8;
          tile.w = 8;
          tile.h = 8;

          draw_rectangle(tile, vec3_white);
        }
      }
    }
  }
  draw_virtual_end();

  draw_begin();

  {
    draw_sprite(rect_init(0, 0, getWindowWidth(), getWindowHeight()), draw.virtualTex);
  }
}

void game_clean() {
  draw_clean();

  platform = 0;
}

void game_sound(void* userdata, uint8* stream, int len) {
  mixer_writeAudio((int16*) stream, len / sizeof(uint16));
}
