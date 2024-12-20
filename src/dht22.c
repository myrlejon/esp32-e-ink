#include "def.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <stdio.h>
#include "esp_timer.h"

#define DHT_22_PIN 32
#define DHT_22_TIMEOUT 100

uint8_t data[5];

uint8_t* dht22_read(void) {

    // start signal and setup
    gpio_set_direction(DHT_22_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT_22_PIN, 0); //pull low for 1ms
    esp_rom_delay_us(1000); // vTaskdelay fungerar INTE om du inte har FreeRTOS uppsatt
    gpio_set_level(DHT_22_PIN, 1); // pull high and wait 20-40μs
    esp_rom_delay_us(30);
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
        data[i / 8] <<= 1;
        if (duration > 50) {
            data[i / 8] |= 1;
        }
    }

    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
    ESP_LOGI("dht22:", "checksum error");
    return 0;
    }

    float humidity = ((data[0] << 8) | data[1]) * 0.1;
    float temperature = ((data[2] & 0x7F) << 8 | data[3]) * 0.1;
    if (data[2] & 0x80) temperature *= -1;  // Negative temperature

    ESP_LOGI("dht22:", "Humidity: %.1f%%", humidity);
    ESP_LOGI("dht22:", "Temperature: %.1f°C", temperature);

    return data;
}