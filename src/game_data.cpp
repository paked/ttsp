struct Actor {
  AABB collider;

  vec2 remainder;

  bool onGround;
};

void actor_moveX(Actor* actor, real32 amount, Level* level) {
  actor->remainder.x += amount;

  int32 move = (int) floorf(actor->remainder.x);
  int32 sign = math_signi(move);

  if (move != 0) {
    actor->remainder.x -= move;

    while (move != 0) {
      AABB r = actor->collider;
      r.x += sign;

      if (!level_isOverlapping(level, r)) {
        actor->collider.x += sign;
        move -= sign;

        continue;
      }

      break;
    }
  }
}

void actor_moveY(Actor* actor, real32 amount, Level* level) {
  actor->remainder.y += amount;

  int32 move = (int) floorf(actor->remainder.y);
  int32 sign = math_signi(move);

  if (move != 0) {
    actor->remainder.y -= move;

    while (move != 0) {
      AABB r = actor->collider;
      r.y += sign;

      if (!level_isOverlapping(level, r)) {
        actor->collider.y += sign;
        move -= sign;

        continue;
      }

      actor->onGround = true;

      break;
    }
  }
}

struct GameData {
  Actor player;
  vec2 playerSpeed;
  real32 jumpAccel;

  Level level;

  MemoryArena memoryArena;
};
