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

    // total of 5 retries
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

    // esp_rom_delay_us(160);

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

    if (dht22_data[4] != ((dht22_data[0] + dht22_data[1] + dht22_data[2] + dht22_data[3]) & 0xFF)) {
        ESP_LOGI("dht22:", "checksum error");
        ESP_LOGI("dht22:", "retrying!");
    } else {
        ESP_LOGI("dht22:", "correct checksum");
    }
    
    // esp_rom_delay_us(1000000);

    float humidity = ((dht22_data[0] << 8) | dht22_data[1]) * 0.1;
    float temperature = ((dht22_data[2] & 0x7F) << 8 | dht22_data[3]) * 0.1;
    if (dht22_data[2] & 0x80) temperature *= -1;  // Negative temperature

    ESP_LOGI("dht22:", "Humidity: %.1f%%", humidity);
    ESP_LOGI("dht22:", "Temperature: %.1f°C", temperature); // sometimes logs exactly HALF of the temp/humidity upon restart, but why? one of the bits gone perhaps?
    ESP_LOGI("dht22:", "dht22_data[0] - %d", dht22_data[0]);
    ESP_LOGI("dht22:", "dht22_data[1] - %d", dht22_data[1]);
    ESP_LOGI("dht22:", "dht22_data[2] - %d", dht22_data[2]);
    ESP_LOGI("dht22:", "dht22_data[3] - %d", dht22_data[3]);



    // TODO
    // save temp and humidity to nvs
    // int restart_count = read_write_nvs("restart_count", -1, "storage");
    // int value = read_write_nvs("temperature", temperature, "storage");


    // draw temp on display
    char temp_buffer[16];
    snprintf(temp_buffer, sizeof(temp_buffer), "%.1f", temperature);
    char hum_buffer[16];
    snprintf(hum_buffer, sizeof(hum_buffer), "%.1f", humidity);

    int temp_first_digit = temp_buffer[0] - '0';
    int temp_second_digit = temp_buffer[1] - '0';
    int temp_decimal_digit = temp_buffer[3] - '0';

    int hum_first_digit = hum_buffer[0] - '0';
    int hum_second_digit = hum_buffer[1] - '0';
    int hum_decimal_digit = hum_buffer[3] - '0';

    int highest_temp_first_digit = read_write_nvs("highest_temp_1", 2, "storage");
    int highest_temp_second_digit = read_write_nvs("highest_temp_second_digit", 2, "storage");
    int highest_temp_decimal_digit = read_write_nvs("highest_temp_decimal_digit", 4, "storage");

    float highest_temp = (highest_temp_first_digit * 10) + highest_temp_second_digit + (highest_temp_decimal_digit / 10.0f);

    if (temperature > highest_temp) {
        highest_temp_first_digit = read_write_nvs("highest_temp_first_digit", temp_first_digit, "storage");
        highest_temp_second_digit = read_write_nvs("highest_temp_second_digit", temp_second_digit, "storage");
        highest_temp_decimal_digit = read_write_nvs("highest_temp_decimal_digit", temp_decimal_digit, "storage");
    }

    // int lowest_temp_first_digit = read_write_nvs("lowest_temp_first_digit", -1, "storage");
    // int lowest_temp_second_digit = read_write_nvs("lowest_temp_second_digit", -1, "storage");
    // int lowest_temp_decimal_digit = read_write_nvs("lowest_temp_decimal_digit", -1, "storage");

    // float lowest_temp = (lowest_temp_first_digit * 10) + lowest_temp_second_digit + (lowest_temp_decimal_digit / 10.0f);

    // if (temperature < lowest_temp) {
    //     lowest_temp_first_digit = read_write_nvs("lowest_temp_first_digit", temp_first_digit, "storage");
    //     lowest_temp_second_digit = read_write_nvs("lowest_temp_second_digit", temp_second_digit, "storage");
    //     lowest_temp_decimal_digit = read_write_nvs("lowest_temp_decimal_digit", temp_decimal_digit, "storage");
    // }

    // int highest_hum_first_digit = read_write_nvs("highest_hum_first_digit", -1, "storage");
    // int highest_hum_second_digit = read_write_nvs("highest_hum_second_digit", -1, "storage");
    // int highest_hum_decimal_digit = read_write_nvs("highest_hum_decimal_digit", -1, "storage");

    // float highest_hum = (highest_hum_first_digit * 10) + highest_hum_second_digit + (highest_hum_decimal_digit / 10.0f);

    // if (humidity > highest_hum) {
    //     highest_hum_first_digit = read_write_nvs("highest_hum_first_digit", hum_first_digit, "storage");
    //     highest_hum_second_digit = read_write_nvs("highest_hum_second_digit", hum_second_digit, "storage");
    //     highest_hum_decimal_digit = read_write_nvs("highest_hum_decimal_digit", hum_decimal_digit, "storage");
    // }

    // int lowest_hum_first_digit = read_write_nvs("lowest_hum_first_digit", -1, "storage");
    // int lowest_hum_second_digit = read_write_nvs("lowest_hum_second_digit", -1, "storage");
    // int lowest_hum_decimal_digit = read_write_nvs("lowest_hum_decimal_digit", -1, "storage");

    // float lowest_hum = (lowest_hum_first_digit * 10) + lowest_hum_second_digit + (lowest_hum_decimal_digit / 10.0f);

    // if (humidity > lowest_hum) {
    //     lowest_hum_first_digit = read_write_nvs("lowest_hum_first_digit", hum_first_digit, "storage");
    //     lowest_hum_second_digit = read_write_nvs("lowest_hum_second_digit", hum_second_digit, "storage");
    //     lowest_hum_decimal_digit = read_write_nvs("lowest_hum_decimal_digit", hum_decimal_digit, "storage");
    // }

    ESP_LOGI("dht22:", "highest temp %d%d.%d", highest_temp_first_digit, highest_temp_second_digit, highest_temp_decimal_digit);


    // temp
    draw_small_number(temp_first_digit, 1, 50, 45);
    draw_small_number(temp_second_digit, 2, 50, 45);
    draw_rect(55, 78, 2, 2); // dot
    draw_small_number(temp_decimal_digit, 3, 50, 45);

    // record high temp
    draw_small_number(highest_temp_first_digit, 1, 50, 117);
    draw_small_number(highest_temp_second_digit, 2, 50, 117);
    draw_rect(55, 150, 2, 2); // dot
    draw_small_number(highest_temp_decimal_digit, 3, 50, 117);

    // // record low temp
    // draw_small_number(lowest_temp_first_digit, 1, 50, 187);
    // draw_small_number(lowest_temp_second_digit, 2, 50, 187);
    // draw_rect(55, 220, 2, 2); // dot
    // draw_small_number(lowest_temp_decimal_digit, 3, 50, 187);

    // humidity
    draw_small_number(hum_first_digit, 1, 10, 45);
    draw_small_number(hum_second_digit, 2, 10, 45);
    draw_rect(15, 78, 2, 2); // dot
    draw_small_number(hum_decimal_digit, 3, 10, 45);

    // // record high humidity
    // draw_small_number(highest_hum_first_digit, 1, 10, 117);
    // draw_small_number(highest_hum_second_digit, 2, 10, 117);
    // draw_rect(15, 150, 2, 2); // dot
    // draw_small_number(highest_hum_decimal_digit, 3, 10, 117);

    // // record low humidity
    // draw_small_number(lowest_hum_first_digit, 1, 10, 187);
    // draw_small_number(lowest_hum_second_digit, 2, 10, 187);
    // draw_rect(15, 220, 2, 2); // dot
    // draw_small_number(lowest_hum_decimal_digit, 3, 10, 187);
}