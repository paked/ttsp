struct Track {
  // The sound asset to play
  // TODO(harrison): When a proper asset system exists, replace this pointer
  // with another identifier.
  Sound* sound;

  // Playback settings
  bool loop;
  real32 volume;

  // Mixing information
  bool playing;
  int16* head;
  uint32 remaining; // samples remaining, note bytes remaining
};

void track_setup(Track* track) {
  track->playing = true;
  track->head = (int16*) track->sound->data;
  track->remaining = track->sound->len / sizeof(int16);
}

#define MAX_TRACK_COUNT (16)
#define CHANNEL_SAMPLE_COUNT (AUDIO_SAMPLE_COUNT/2)

// Mixer is a simple 16 track (by default) audio mixer. It's easy to read, easy
// to use, and not that fast. Although, it probably doesn't need to be. Maybe
// one day I'll rewrite it with SIMD.
struct Mixer {
  real32 masterVolume;

  Track tracks[MAX_TRACK_COUNT];

  int16 rightChannel[CHANNEL_SAMPLE_COUNT];
  int16 leftChannel[CHANNEL_SAMPLE_COUNT];
};

Mixer mixer = {0};

void mixer_reset() {
  memset(mixer.rightChannel, 0, sizeof(mixer.rightChannel));
  memset(mixer.leftChannel, 0, sizeof(mixer.leftChannel));
}

void mixer_init() {
  mixer.masterVolume = 1.0f;

  mixer_reset();
}

Track* mixer_getFirstAvailableTrack() {
  for (int i = 0; i < MAX_TRACK_COUNT; i++) {
    Track* t = &mixer.tracks[i];

    if (!t->playing) {
      return t;
    }
  }

  return 0;
}

void mixer_playSound(Sound* sound, bool loop, real32 volume) {
  Track* track = mixer_getFirstAvailableTrack();
  if (track == 0) {
    logln("WARNING: playing too many audio tracks!");

    return;
  }

  track->sound = sound;
  track->loop = loop;
  track->volume = volume;

  track_setup(track);

  logln("playing sound!");
}

void mixer_writeAudio(int16* stream, int totalLen) {
  assert(totalLen == AUDIO_SAMPLE_COUNT);

  mixer_reset();

  for (int i = 0; i < MAX_TRACK_COUNT; i++) {
    int len = totalLen;

    Track* track = &mixer.tracks[i];

    if (!track->playing) {
      continue;
    }

    if ((uint32) len > track->remaining) {
      len = track->remaining;
    }

    int leftChannelIndex = 0;
    int rightChannelIndex = 0;

    int j = 0;
    while (j < len) {
      mixer.leftChannel[leftChannelIndex] = (*track->head) * track->volume;

      leftChannelIndex += 1;
      track->head += 1;
      track->remaining -= 1;

      mixer.rightChannel[rightChannelIndex] = (*track->head) * track->volume;

      rightChannelIndex += 1;
      track->head += 1;
      track->remaining -= 1;

      j += 2;
    }

    if (track->remaining <= 0) {
      track->playing = false;

      if (track->loop) {
        track_setup(track);
      }
    }
  }

  int leftChannelIndex = 0;
  int rightChannelIndex = 0;
  for (int i = 0; i < totalLen; i += 2) {
    stream[i] = mixer.leftChannel[leftChannelIndex] * mixer.masterVolume;
    leftChannelIndex += 1;

    stream[i + 1] = mixer.rightChannel[rightChannelIndex] * mixer.masterVolume;
    rightChannelIndex += 1;
  }
}
