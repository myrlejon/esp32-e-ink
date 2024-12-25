#include "def.h"
#include "esp_sleep.h"
#include "esp_log.h"

#define SLEEP_TIME 1 // minutes

void mcu_sleep(void) {
    esp_rom_delay_us(1000);
    esp_sleep_enable_timer_wakeup(SLEEP_TIME * 60 * 1000000); // minutes * seconds * 10^6 (convert microseconds > seconds)
    ESP_LOGI("sleep:", "going to sleep for %d minutes", SLEEP_TIME);
    esp_deep_sleep_start();
    ESP_LOGI("sleep:", "wake up, restarting");
    // esp_restart();
}