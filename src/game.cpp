// Technically we don't really need this reference, since we're still in the
// same compilation unit as the "runner" but it makes it more explicit that
// there is a CLEAN distinction between runner and game code.
Platform* platform = 0;

int game_init(Platform* p) {
  platform = p;

  return 0;
}

void game_update() {
  assert(platform != 0);

  glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void game_clean() {
  platform = 0;
}
