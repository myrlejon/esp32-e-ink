#include "def.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <image.h>
#include "esp_task_wdt.h"

#define DISPLAY_WIDTH 128 // 122 pixels wide but 122/8 = 15.25, so the last 6 bits are padded therefore 128 is used
#define DISPLAY_HEIGHT 256

#define WHITE 0xFF
#define BLACK 0x00

uint8_t byte_array[4000];

// char byteArray[DISPLAY_WIDTH][DISPLAY_HEIGHT];
// uint8_t byteArray[(250 * 122) / 8] = { 0x00 };
// uint8_t byteArray[3813];

// set all bits to 1 (white)
uint8_t bitArray[DISPLAY_WIDTH][DISPLAY_HEIGHT];
// memset(bitArray, 1, sizeof(bitArray));

void draw_pixel(int x, int y, uint8_t color) {
    int bit_index = (y * DISPLAY_WIDTH) + x;
    int byte_index = bit_index / 8;
    int bit_offset = bit_index % 8;

    if (color == WHITE) {
        byte_array[byte_index] |= (uint8_t)(1 << bit_offset);
    } else {
        byte_array[byte_index] &= ~((uint8_t)(1 << bit_offset));
    }
} 

void draw_rect(int xPos, int yPos, int width, int height) {
    ESP_LOGI("draw_rect:", "starting...");

    for (int i = 0; i < DISPLAY_WIDTH; i++) {
        for (int j = 0; j < DISPLAY_HEIGHT; j++) {
            if (i > xPos && i < xPos + width &&
                j > yPos && j < yPos + height)
                {
                    draw_pixel(i, j, WHITE);
                    // ESP_LOGI("draw_rect:", "wrote pixel at: x: %d y: %d", i, j);
                }
        }
    }

    ESP_LOGI("draw_rect:", "sending data");
    send_command(0x24);
    for (int i = 0; i < sizeof(byte_array); i++) {
        send_data(byte_array[i]);
    }
    // ESP_LOGI("draw_rect:", "done");
}

void draw_command() {
    send_command(0x24);
    for (int i = 0; i < sizeof(byte_array); i++) {
        send_data(byte_array[i]);
    }
}

// image gets inverted (negative) pixels, probably a little/big endian problem in the python script
void write_image_txt_to_display(void) {
    send_command(0x24);
    int imageArraySize = sizeof(image_array) / sizeof(image_array[0]);

    ESP_LOGI("image info:", "array size: %d", imageArraySize);
    ESP_LOGI("test", "%d", image_array[1]);

    for (int i = 0; i < imageArraySize; i++) {
        if (i % 100 == 0) {
            ESP_LOGI("image info:", "0x%02X", image_array[i]);
        }
        send_data(image_array[i]);
    }
}