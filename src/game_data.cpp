struct Actor {
  AABB collider;

  vec2 remainder;
};

void actor_moveX(Actor* actor, real32 amount, AABB ground) {
  actor->remainder.x += amount;

  int32 move = (int) floorf(actor->remainder.x);
  int32 sign = math_signi(move);

  if (move != 0) {
    actor->remainder.x -= move;

    while (move != 0) {
      AABB r = actor->collider;
      r.x += sign;

      if (!physics_isOverlapping(r, ground)) {
        actor->collider.x += sign;
        move -= sign;

        continue;
      }

      break;
    }
  }
}

void actor_moveY(Actor* actor, real32 amount, AABB ground) {
  actor->remainder.y += amount;

  int32 move = (int) floorf(actor->remainder.y);
  int32 sign = math_signi(move);

  if (move != 0) {
    actor->remainder.y -= move;

    while (move != 0) {
      AABB r = actor->collider;
      r.y += sign;

      if (!physics_isOverlapping(r, ground)) {
        actor->collider.y += sign;
        move -= sign;

        continue;
      }

      break;
    }
  }
}

struct GameData {
  Actor player;
  real32 playerSpeed;
  real32 jumpAccel;

  AABB ground;

  MemoryArena memoryArena;
};
