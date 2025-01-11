#include "def.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <stdio.h>
#include <string.h>
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

    set_record_temp("ht", temperature, true, 1); // highest temperature
    set_record_temp("lt", temperature, false, 2); // lowest temperature
    set_record_temp("hh", humidity, true, 3); // highest humidity
    set_record_temp("lh", humidity, false, 4); // lowest humidity

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

    // temp
    draw_small_number(temp_first_digit, 1, 50, 45);
    draw_small_number(temp_second_digit, 2, 50, 45);
    draw_rect(55, 78, 2, 2); // dot
    draw_small_number(temp_decimal_digit, 3, 50, 45);

    // humidity
    draw_small_number(hum_first_digit, 1, 10, 45);
    draw_small_number(hum_second_digit, 2, 10, 45);
    draw_rect(15, 78, 2, 2); // dot
    draw_small_number(hum_decimal_digit, 3, 10, 45);
}

void set_record_temp(char key_name[], float value, bool high, int position) {
    char first_digit[4] = "fd";
    char second_digit[4] = "sd";
    char decimal_digit[4] = "dd";
    
    strcat(first_digit, key_name);
    strcat(second_digit, key_name);
    strcat(decimal_digit, key_name);

    int first_digit_record_value = read_write_nvs(first_digit, -1, "storage");
    int second_digit_record_value = read_write_nvs(second_digit, -1, "storage");
    int decimal_digit_record_value = read_write_nvs(decimal_digit, -1, "storage");

    float record_value = (first_digit_record_value * 10) + second_digit_record_value + (decimal_digit_record_value / 10.0f);

    if ((record_value > value && high) || (record_value < value && !high)) {
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%.1f", value);

        int first_digit_value = buffer[0] - '0';
        int second_digit_value = buffer[1] - '0';
        int decimal_digit_value = buffer[3] - '0';

        read_write_nvs(first_digit, first_digit_value, "storage");
        read_write_nvs(second_digit, second_digit_value, "storage");
        read_write_nvs(decimal_digit, decimal_digit_value, "storage");
    }

    switch (position)
    {
        case 1:
            draw_small_number(first_digit_record_value, 1, 50, 117);
            draw_small_number(second_digit_record_value, 2, 50, 117);
            draw_rect(55, 150, 2, 2); // dot
            draw_small_number(decimal_digit_record_value, 3, 50, 117);
            break;
        case 2:
            draw_small_number(first_digit_record_value, 1, 50, 187);
            draw_small_number(second_digit_record_value, 2, 50, 187);
            draw_rect(55, 220, 2, 2); // dot
            draw_small_number(decimal_digit_record_value, 3, 50, 187);
            break;
        case 3:
            draw_small_number(first_digit_record_value, 1, 10, 117);
            draw_small_number(second_digit_record_value, 2, 10, 117);
            draw_rect(15, 150, 2, 2); // dot
            draw_small_number(decimal_digit_record_value, 3, 10, 117);
            break;
        case 4:
            draw_small_number(first_digit_record_value, 1, 10, 187);
            draw_small_number(second_digit_record_value, 2, 10, 187);
            draw_rect(15, 220, 2, 2); // dot
            draw_small_number(decimal_digit_record_value, 3, 10, 187);
            break;
    }
}