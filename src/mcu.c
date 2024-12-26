#include "def.h"
#include "esp_sleep.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SLEEP_TIME 15 // minutes

void mcu_sleep(void) {
    // esp_sleep_enable_timer_wakeup(SLEEP_TIME * 60 * 1000000); // minutes * seconds * 10^6 (convert microseconds > seconds)
    // ESP_LOGI("sleep:", "going to sleep for %d minutes", SLEEP_TIME);
    // esp_deep_sleep_start();

    // TODO - fix so that MCU goes to sleep, right now it just waits for 15 minutes then resets

    // esp_rom_delay_us(1000000);
    ESP_LOGI("sleep:", "waiting 15 minutes then restart");
    vTaskDelay(pdMS_TO_TICKS(1000 * SLEEP_TIME * 60));
    ESP_LOGI("sleep:", "restarting");
    esp_restart();

    // ESP_LOGI("sleep:", "wake up, restarting");
}