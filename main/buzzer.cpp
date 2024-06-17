#include "./include/buzzer.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <cmath>
#include <stdio.h>
#include <unistd.h>

#include "esp_sleep.h"
#include <string.h>

// Frequency in Hertz. Set frequency by buzzer
#define LEDC_FREQUENCY (2700)

#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#ifdef CONFIG_IDF_TARGET_ESP32S3
#define LEDC_OUTPUT_IO GPIO_NUM_13  // for esp32-s3 board
#else
#define LEDC_OUTPUT_IO GPIO_NUM_12  // for esp32 board
#define LEDC_CHANNEL LEDC_CHANNEL_0

// Duty resolution document: https://www.espressif.com.cn/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf#table.14.1
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT  // Set duty resolution to 13 bits

static bool _playing = false;

// get duty by level
static uint32_t get_duty_by_level(uint32_t level);

void buzzer_init(void) {
  // Prepare and then apply the LEDC PWM timer configuration
  ledc_timer_config_t ledc_timer;
  ledc_timer.speed_mode      = LEDC_MODE;
  ledc_timer.timer_num       = LEDC_TIMER;
  ledc_timer.duty_resolution = LEDC_DUTY_RES;
  ledc_timer.freq_hz         = LEDC_FREQUENCY;  // Set output frequency at 5 kHz
  ledc_timer.clk_cfg         = LEDC_AUTO_CLK;

  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  // Prepare and then apply the LEDC PWM channel configuration
  ledc_channel_config_t ledc_channel;
  ledc_channel.speed_mode = LEDC_MODE;
  ledc_channel.channel    = LEDC_CHANNEL;
  ledc_channel.timer_sel  = LEDC_TIMER;
  ledc_channel.intr_type  = LEDC_INTR_DISABLE;
  ledc_channel.gpio_num   = LEDC_OUTPUT_IO;
  ledc_channel.duty       = 4000;  // Set duty to 0%
  ledc_channel.hpoint     = 0;

  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void buzzer_start(void) {
  buzzer_init();
  ESP_LOGW("####", "buzzer_start duty: %d", (int)get_duty_by_level(12));
  // Set duty
  ESP_ERROR_CHECK(
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, get_duty_by_level(12)));
  // Update duty to apply the new value
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
  _playing = true;
}

void buzzer_stop(void) {
  if (!_playing) {
    return;
  }
  ESP_ERROR_CHECK(ledc_stop(LEDC_MODE, LEDC_CHANNEL, 1));

  gpio_config_t io_conf;
  io_conf.intr_type    = GPIO_INTR_DISABLE;
  io_conf.pin_bit_mask = 1ULL << LEDC_OUTPUT_IO;
  io_conf.mode         = GPIO_MODE_INPUT;
  io_conf.pull_down_en = (gpio_pulldown_t)1;  //  pull-down mode
  io_conf.pull_up_en   = (gpio_pullup_t)0;  // pull-up mode
  gpio_config(&io_conf);

  _playing = false;
}

bool buzzer_playing() {
  return _playing;
}

/**
 * @brief Get the duty by level object
 * 
 * 0: match 0
 * 1~10: match pow(2, 2~11), for sound level=1 has nearlly no sound
 */
uint32_t get_duty_by_level(uint32_t level) {
  if (level == 0) {
    return 0;
  }
  return (uint32_t)pow(2, level);
}
