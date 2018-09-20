struct LevelChunk {
  int i;
  int width;
  int height;

  // NOTE(harrison): this *must* be the last element.
  // NOTE(harrison): data is an array of length (width*height)
  int* data;
};

void level_load(MemoryArena* ma) {
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

#define CHUNK_WIDTH 50
  assert(width % CHUNK_WIDTH == 0);

  for (int begin = 0; begin < (width / CHUNK_WIDTH); begin++) {
    LevelChunk* chunk = (LevelChunk*) memoryArena_pushSize(ma, sizeof(LevelChunk) + sizeof(int*) * CHUNK_WIDTH * height);

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
#undef CHUNK_WIDTH

  logfln("level sized: %d x %d", oldWidth, height);
}
