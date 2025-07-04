#pragma once
#include "main.h"

typedef enum{
  TONE_START,
  TONE_END
} tone_cmd_t;

typedef struct{
  tone_cmd_t tone_cmd;
  unsigned int frequency;
  unsigned long duration;
} tone_msg_t;

int tone_init();
void tone_play(unsigned int frequency, unsigned long duration);