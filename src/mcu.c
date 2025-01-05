#include "def.h"
#include "esp_sleep.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "nvs.h"

#define SLEEP_TIME 1 // minutes

void mcu_sleep(void) {
    // esp_sleep_enable_timer_wakeup(SLEEP_TIME * 60 * 1000000); // minutes * seconds * 10^6 (convert microseconds > seconds)
    // ESP_LOGI("sleep:", "going to sleep for %d minutes", SLEEP_TIME);
    // esp_deep_sleep_start();

    // TODO - fix so that MCU goes to sleep, right now it just waits for 15 minutes then resets

    // esp_rom_delay_us(1000000);

    // read and write to restart count
    int restart_count = read_write_nvs("restart_count", -1, "storage");
    // reset restart_count every 24 hours (24 * 4, as it wakes up every 15 minutes)
    if (restart_count == 96) {
        restart_count = 0;
    }
    restart_count++;
    ESP_LOGI("sleep:", "restart_count - %d", restart_count);
    read_write_nvs("restart_count", restart_count, "storage");

    ESP_LOGI("sleep:", "waiting %d minutes then restart", SLEEP_TIME);
    vTaskDelay(pdMS_TO_TICKS(1000 * SLEEP_TIME * 60));
    ESP_LOGI("sleep:", "restarting");
    esp_restart();

    // ESP_LOGI("sleep:", "wake up, restarting");
}

// reads (return) and writes (with parameters) from key value pairs
// if value = -1, don't write
int read_write_nvs(const char* key, int value, const char* storage) {
    // https://github.com/espressif/esp-idf/blob/master/examples/storage/nvs_rw_value/main/nvs_value_example_main.c
    // Initialize NVS
    
    int stored_value = 0; // value will default to 0, if not set yet in NVS

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    // Open
    ESP_LOGI("read_write_nvs", "opening NVS handle...");
    nvs_handle_t handle;
    err = nvs_open(storage, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGI("read_write_nvs", "unable to open storage");
    } else {
        ESP_LOGI("read_write_nvs", "opened storage");

        // Read
        ESP_LOGI("read_write_nvs", "reading value from nvs...");
        err = nvs_get_i32(handle, key, &stored_value);
        switch (err) {
            case ESP_OK:
                ESP_LOGI("read_write_nvs", "done");
                ESP_LOGI("read_write_nvs", "value = %d", value);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGI("read_write_nvs", "value is not initialized yet");
                break;
            default :
                ESP_LOGI("read_write_nvs", "error (%s) reading", esp_err_to_name(err));
        }

        // Write
        if (value != -1) {
            ESP_LOGI("read_write_nvs", "writing to nvs...");
            err = nvs_set_i32(handle, key, value);
            ESP_LOGI("read_write_nvs", "%s", (err != ESP_OK ? "failed" : "done"));
            ESP_LOGI("read_write_nvs", "committing value...");
            err = nvs_commit(handle);
            ESP_LOGI("read_write_nvs", "%s", (err != ESP_OK ? "failed" : "done"));
        } else {
            ESP_LOGI("read_write_nvs", "skipping writing process because of -1");
        }

        // Close
        nvs_close(handle);
    }
    return stored_value;
}
