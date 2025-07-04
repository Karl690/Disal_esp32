
#pragma once

#include "main.h"
typedef enum {
    ENC_ACTION_NONE,
    ENC_ACTION_CLOCKWISE,
    ENC_ACTION_COUNTERCLOCKWISE,
} ENC_ACTION_TYPE;

extern volatile int32_t encoder_pos;
extern volatile ENC_ACTION_TYPE encoder_action;
void encoder_init();
void encoder_check();