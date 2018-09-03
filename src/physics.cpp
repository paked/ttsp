// TODO(harrison): move somewhere else?
struct Rect {
  real32 x, y;
  real32 w, h;
};

Rect rect_init(real32 x, real32 y, real32 w, real32 h) {
  Rect r;

  r.x = x;
  r.y = y;
  r.w = w;
  r.h = h;

  return r;
}

struct AABB {
  int32 x, y;
  int32 w, h;
};

AABB aabb_init(int32 x, int32 y, int32 w, int32 h) {
  AABB r;

  r.x = x;
  r.y = y;
  r.w = w;
  r.h = h;

  return r;
}

struct Point {
  int32 x;
  int32 y;
};

bool physics_isOverlapping(AABB first, AABB second) {
  Point amin = { first.x, first.y };
  Point amax = { amin.x + first.w, amin.y + first.h };
  Point bmin = { second.x, second.y };
  Point bmax = { bmin.x + second.w, bmin.y + second.h };

  return bmin.x < amax.x &&
    amin.x < bmax.x &&
    bmin.y < amax.y &&
    amin.y < bmax.y;
}
