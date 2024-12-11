#include "def.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <image.h>
#include "esp_task_wdt.h"

#define DISPLAY_WIDTH 128 // 122 pixels wide but 122/8 = 15.25, so the last 6 bits are padded therefore 128 is used
#define DISPLAY_HEIGHT 250

#define WHITE 0xFF
#define BLACK 0x00

uint8_t byte_array[4000]; // (250 * 128) / 8

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
                }
        }
    }

    // // rectangle should be written the same way as the display sends data to make it more easy
    // // two loops with DISPLAY_WIDTH (250) on the outside and DISPLAY_HEIGHT(122) on the inside 

    // ESP_LOGI("draw_rect:", "setting bits to black");
    // //set bits to black
    // for (int i = 0; i < DISPLAY_WIDTH; i++) {
    //     for (int j = 0; j < DISPLAY_HEIGHT; j++) {
    //         if (i > xPos && i < xPos + width &&
    //             j > yPos && j < yPos + height &&
    //             xPos + width <= DISPLAY_WIDTH &&
    //             yPos + width <= DISPLAY_HEIGHT) {
    //                 bitArray[i][j] = 0; // 0 = black
    //                 ESP_LOGI("display_rect:", "painting black for iteration %d %d", i, j);
    //             }
    //         // if ((i * DISPLAY_HEIGHT + j) % 100 == 0) {
    //         //     esp_task_wdt_reset();
    //         // }
    //     }
    // }

    // uint8_t hex = 0;
    // int bitCounter = 0;
    // int hexCounter = 0;

    // ESP_LOGI("draw_rect:", "converting bits to bytes");
    // //convert bits to bytes 
    // for (int i = 0; i < DISPLAY_WIDTH; i++) {
    //     for (int j = 0; j <  DISPLAY_HEIGHT; j++) {
    //         // gather bytes from rectArray and shift them into hex
            
    //         if (bitArray[i][j] == 0) {
    //             hex = (hex << 1);
    //         }
    //         else {
    //             hex = (hex << 1) | 1;
    //         }
    //         bitCounter++;
            
    //         // put hex into byteArray
    //         if (bitCounter == 8) {
    //             byte_array[hexCounter++] = hex;
    //             hex = 0xFF;
    //             bitCounter = 0;
    //         }
    //     }
    // }

    // // for remaining bits
    // if (bitCounter > 0) {
    //     byteArray[hexCounter] = hex << (8 - bitCounter);
    // }

    // :)

    // draw_pixel(60, 125, WHITE);
    // draw_pixel(60, 126, WHITE);
    // draw_pixel(61, 125, WHITE);
    // draw_pixel(61, 126, WHITE);

    // draw_pixel(60, 150, WHITE);
    // draw_pixel(60, 151, WHITE);
    // draw_pixel(61, 150, WHITE);
    // draw_pixel(61, 151, WHITE);


    // draw_pixel(86, 125, WHITE);
    // draw_pixel(85, 126, WHITE);
    // draw_pixel(84, 127, WHITE);

    // draw_pixel(83, 128, WHITE);
    // draw_pixel(83, 129, WHITE);
    // draw_pixel(83, 130, WHITE);
    // draw_pixel(83, 131, WHITE);
    // draw_pixel(83, 132, WHITE);
    // draw_pixel(83, 133, WHITE);
    // draw_pixel(83, 134, WHITE);
    // draw_pixel(83, 135, WHITE);
    // draw_pixel(83, 136, WHITE);
    // draw_pixel(83, 137, WHITE);
    // draw_pixel(83, 138, WHITE);
    // draw_pixel(83, 139, WHITE);
    // draw_pixel(83, 140, WHITE);
    // draw_pixel(83, 141, WHITE);
    // draw_pixel(83, 142, WHITE);
    // draw_pixel(83, 143, WHITE);
    // draw_pixel(83, 144, WHITE);
    // draw_pixel(83, 145, WHITE);
    // draw_pixel(83, 146, WHITE);
    // draw_pixel(83, 147, WHITE);
    // draw_pixel(83, 148, WHITE);
    // draw_pixel(84, 149, WHITE);
    // draw_pixel(85, 150, WHITE);
    // draw_pixel(86, 151, WHITE);


    // byte_array[15] = 0xFF;

    ESP_LOGI("draw_rect:", "sending data");
    send_command(0x24);
    for (int i = 0; i < sizeof(byte_array); i++) {
        send_data(byte_array[i]);
    }
    // ESP_LOGI("draw_rect:", "done");
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