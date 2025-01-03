#include "def.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <stdio.h>
#include "esp_timer.h"
#include <time.h>
#include "nvs_flash.h"
#include "nvs.h"

#define DHT_22_PIN 32
#define DHT_22_TIMEOUT 100

void dht22_read(void) {
    uint8_t dht22_data[5];

    // start signal and setup
    gpio_set_direction(DHT_22_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT_22_PIN, 0); //pull low for 1ms
    esp_rom_delay_us(1000); // vTaskdelay fungerar INTE om du inte har FreeRTOS uppsatt
    gpio_set_level(DHT_22_PIN, 1); // pull high and wait 20-40μs
    esp_rom_delay_us(40);
    gpio_set_direction(DHT_22_PIN, GPIO_MODE_INPUT);

    if (gpio_get_level(DHT_22_PIN) == 1) {
        ESP_LOGI("dht22:", "no response");
    } else {
        ESP_LOGI("dht22:", "response!");
    }


    for (int i = 0; i < 40; i++) {
        while (gpio_get_level(DHT_22_PIN) == 0);  // wait for low
        uint32_t start_time = esp_timer_get_time(); // for measuring high duration

        while (gpio_get_level(DHT_22_PIN) == 1); 
        uint32_t duration = esp_timer_get_time() - start_time;

        // higher than 50µs is a high 1 bit, otherwise low 0 bit.
        dht22_data[i / 8] <<= 1;
        if (duration > 50) {
            dht22_data[i / 8] |= 1;
        }
    }

    // esp_rom_delay_us(1000000);

    if (dht22_data[4] != ((dht22_data[0] + dht22_data[1] + dht22_data[2] + dht22_data[3]) & 0xFF)) {
        ESP_LOGI("dht22:", "checksum error");
        return;
    }

    float humidity = ((dht22_data[0] << 8) | dht22_data[1]) * 0.1;
    float temperature = ((dht22_data[2] & 0x7F) << 8 | dht22_data[3]) * 0.1;
    if (dht22_data[2] & 0x80) temperature *= -1;  // Negative temperature

    ESP_LOGI("dht22:", "Humidity: %.1f%%", humidity);
    ESP_LOGI("dht22:", "Temperature: %.1f°C", temperature);

    // draw temp on display
    char temp_buffer[16];
    snprintf(temp_buffer, sizeof(temp_buffer), "%.1f", temperature);

    int temp_first_digit = temp_buffer[0] - '0';
    int temp_second_digit = temp_buffer[1] - '0';
    int temp_decimal_digit = temp_buffer[3] - '0';

    // temp
    draw_small_number(temp_first_digit, 1, 50, 45);
    draw_small_number(temp_second_digit, 2, 50, 45);
    draw_rect(55, 78, 2, 2); // dot
    draw_small_number(temp_decimal_digit, 3, 50, 45);

    char hum_buffer[16];
    snprintf(hum_buffer, sizeof(hum_buffer), "%.1f", humidity);

    int hum_first_digit = hum_buffer[0] - '0';
    int hum_second_digit = hum_buffer[1] - '0';
    int hum_decimal_digit = hum_buffer[3] - '0';

    // humidity
    draw_small_number(hum_first_digit, 1, 10, 45);
    draw_small_number(hum_second_digit, 2, 10, 45);
    draw_rect(15, 78, 2, 2); // dot
    draw_small_number(hum_decimal_digit, 3, 10, 45);

    record_temp(10);
}

// gets the highest/lowest temp from the past 24 hours
void record_temp(int current_temp) {

    // https://github.com/espressif/esp-idf/blob/master/examples/storage/nvs_rw_value/main/nvs_value_example_main.c
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    // Open
    ESP_LOGI("record_temp", "opening NVS handle...");
    nvs_handle_t handle;
    err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");

        // Read
        printf("Reading restart counter from NVS ... ");
        int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
        err = nvs_get_i32(handle, "restart_counter", &restart_counter);
        switch (err) {
            case ESP_OK:
                printf("Done\n");
                printf("Restart counter = %" PRIu32 "\n", restart_counter);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

        // Write
        printf("Updating restart counter in NVS ... ");
        restart_counter++;
        err = nvs_set_i32(handle, "restart_counter", restart_counter);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Close
        nvs_close(handle);
    }

    nvs_handle_t nvs_handle;
    nvs_open("temp_storage", NVS_READWRITE, &nvs_handle);
    nvs_set_i32(nvs_handle, "current_value", current_temp);
    nvs_commit(nvs_handle);


    int highest_temp = 0;
    nvs_get_i32(nvs_handle, "current_value", &highest_temp);

    // time_t now = time(NULL);
    ESP_LOGI("record_temp", "current temp - %d", highest_temp);
}
