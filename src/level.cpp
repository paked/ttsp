struct Level {
  MemoryArena* arena;
};

#define CHUNK_WIDTH (50)
struct LevelChunk {
  int i;
  int width;
  int height;

  // NOTE(harrison): this *must* be the last element.
  // NOTE(harrison): data is an array of length (width*height)
  int* data;
};

void level_init(Level* level, MemoryArena* arena) {
  level->arena = arena;
}

void level_load(Level* level) {
  assert(level->arena != 0);

  void* fileData;
  psize fileLen;

  const char* fname = "map/level.csv";

  mustLoadFromFile((char*) fname, &fileData, &fileLen);

  int oldWidth = -1;
  int width = 0;
  int height = 0;

  int* temp = (int*) platform->transientStorage;
  psize head = 0;

  psize i = 0;
  char* data = (char*) fileData;

  bool linebreak = false;

  while (i < fileLen) {
    char* cell = data + i;
    psize len = 0;

    while (i < fileLen) {
      char c = (cell + len)[0];
      if (c == ',') {
        break;
      } else if (us_isNewline(c)) {
        linebreak = true;

        break;
      }

      len += 1;

      i += 1;
    }

    width += 1;

    if (linebreak) {
      height += 1;

      if (oldWidth == -1) {
        oldWidth = width;
      } else {
        assert(width == oldWidth);
      }

      width = 0;

      linebreak = false;
    }

    int v = us_parseInt(cell, len);
    temp[head] = v;

    head += 1;

    assert(sizeof(int) * head < TRANSIENT_MEMORY_SIZE);

    i += 1;
  }

  width = oldWidth;

  assert(width % CHUNK_WIDTH == 0);

  for (int begin = 0; begin < (width / CHUNK_WIDTH); begin++) {
    LevelChunk* chunk = (LevelChunk*) memoryArena_pushSize(level->arena, sizeof(LevelChunk) + sizeof(int*) * CHUNK_WIDTH * height);

    chunk->i = begin;
    chunk->width = CHUNK_WIDTH;
    chunk->height = height;
    chunk->data = ((int*) chunk) + (sizeof(*chunk) - sizeof(chunk->data));

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < CHUNK_WIDTH; x++) {
        int offset = begin * CHUNK_WIDTH + x;

        int* from = ((int*) platform->transientStorage) + y * width + offset;

        int* to = chunk->data + y * chunk->width + x;

        if (begin == 0) {
          printf(" %d ", *from);
        }

        *to = *from;
      }

      if (begin == 0) {
        printf("\n");
      }
    }
  }
  // TODO(harrison): this chunking logic will drop the remaining rows from width % CHUNK_WIDTH

  logfln("level sized: %d x %d", oldWidth, height);
}

bool levelChunk_isOverlapping(LevelChunk* chunk, AABB collider) {
  AABB tile = {0};
  tile.w = 8;
  tile.h = 8;

  for (int y = 0; y < chunk->height; y++) {
    for (int x = 0; x < chunk->width; x++) {
      int* v = chunk->data + y * chunk->width + x;

      if (*v < 0) {
        continue;
      }

      tile.x = (chunk->i * CHUNK_WIDTH + x) * 8;
      tile.y = y * 8;

      if (physics_isOverlapping(collider, tile)) {
        return true;
      }
    }
  }

  return false;
}

LevelChunk* level_getChunk(Level* level, int target) {
  int i = 0;
  for (MemoryBlock* block = level->arena->first; block != 0; block = block->next) {
    if (i == target) {
      return (LevelChunk*) block->start;
    }

    i += 1;
  }

  return 0;
}

bool level_isOverlapping(Level* level, AABB collider) {
  // Chunk that the left hand side of the collider is in
  int min = collider.x / 8.0f / CHUNK_WIDTH;
  // Chunk that the right hand side of the collider is in
  int max = (collider.x + collider.w) / 8.0f / CHUNK_WIDTH;

  for (int i = min; i <= max; i++) {
    LevelChunk* chunk = level_getChunk(level, i);

    assert(chunk != 0);

    if (levelChunk_isOverlapping(chunk, collider)) {
      return true;
    }
  }

  return false;
}
