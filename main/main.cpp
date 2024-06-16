#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_freertos_hooks.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#define USE_LV_LOG 1

#include <buzzer.h>

#define _GUI_TAG "GUI_TASK"

static void hardware_init();

extern "C" {
void app_main();
}

static void gui_task(void*);  // UI初始化

void app_main() {
  hardware_init();

  xTaskCreatePinnedToCore(gui_task, "gui_task", 1024 * 16, NULL, 5, NULL, 0);
}

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t gui_semaphore;

static void gui_task(void* arg) {
  (void)arg;
  ESP_LOGI(_GUI_TAG, "gui_task started");
  gui_semaphore = xSemaphoreCreateMutex();

  ESP_LOGW("####", "melody_buzzer_play");
  buzzer_start();

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(100));
  }

#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
#endif
  vTaskDelete(NULL);
}

// 设备初始化
static void hardware_init() {
  // 初始化 NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
}
