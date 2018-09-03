#include <assert.h>

#define MAX_PATH (128)

#define log(...) fprintf(stderr, __VA_ARGS__)
#define logln(fmt) fprintf(stderr, fmt "\n")
#define logfln(fmt, ...) fprintf(stderr, fmt "\n", __VA_ARGS__)

#define bytes(n)     (n)
#define kilobytes(n) (bytes(n)*1024)
#define megabytes(n) (kilobytes(n)*1024)
#define gigabytes(n) (megabytes(n)*1024)

typedef int8_t    int8;
typedef int16_t   int16;
typedef int32_t   int32;
typedef int64_t   int64;
typedef uint8_t   uint8;
typedef uint16_t  uint16;
typedef uint32_t  uint32;
typedef uint64_t  uint64;
typedef float     real32;
typedef double    real64;
typedef int32     bool32;

typedef size_t MemoryIndex;

enum {
  KEY_unknown,

  KEY_0, KEY_1, KEY_2, KEY_3, KEY_4,
  KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,

  KEY_shift, KEY_grave, KEY_ctrl, KEY_tab, KEY_space,

  KEY_a,
  KEY_b,
  KEY_c,
  KEY_d,
  KEY_e,
  KEY_f,
  KEY_g,
  KEY_h,
  KEY_i,
  KEY_j,
  KEY_k,
  KEY_l,
  KEY_m,
  KEY_n,
  KEY_o,
  KEY_p,
  KEY_q,
  KEY_r,
  KEY_s,
  KEY_t,
  KEY_u,
  KEY_v,
  KEY_w,
  KEY_x,
  KEY_y,
  KEY_z,

  MAX_KEY
};

typedef int (* LoadFromFileFunc)(char* fname, void** buffer, MemoryIndex* size);

struct Platform {
  // Memory related things
  bool initialized;

  uint64 permanentStorageSize;
  void* permanentStorage;
  uint64 transientStorageSize;
  void* transientStorage;

  // IO
  bool quit;

  bool keyStateNow[MAX_KEY];
  bool keyStateLast[MAX_KEY];

  // Timing
  uint32 time;
  real32 deltaTime;

  real64 currentFPS;
  uint32 targetFPS;

  // Window
  uint32 windowWidth;
  uint32 windowHeight;

  // Functions
  LoadFromFileFunc loadFromFile;
};
