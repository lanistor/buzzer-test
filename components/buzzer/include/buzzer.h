#pragma once

#include "esp_timer.h"
#include <stdbool.h>
#include <stdint.h>

void buzzer_init(void);

void buzzer_start(void);

void buzzer_stop(void);

bool buzzer_playing();
