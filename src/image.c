#include "def.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <image.h>
#include "esp_task_wdt.h"

#define DISPLAY_WIDTH 250
#define DISPLAY_HEIGHT 122

// char byteArray[DISPLAY_WIDTH][DISPLAY_HEIGHT];

void draw_rect(int xPos, int yPos, int width, int height) {
    uint8_t byteArray[(250 * 122) / 8] = { 0xFF };
    // set all bits to 1 (white)
    uint8_t bitArray[DISPLAY_WIDTH][DISPLAY_HEIGHT] = {{1}};
    memset(bitArray, 1, sizeof(bitArray));

    // rectangle should be written the same way as the display sends data to make it more easy
    // two loops with DISPLAY_WIDTH (250) on the outside and DISPLAY_HEIGHT(122) on the inside 

    ESP_LOGI("display_rect:", "setting bits to black");
    //set bits to black
    for (int i = 0; i < DISPLAY_WIDTH; i++) {
        for (int j = 0; j < DISPLAY_HEIGHT; j++) {
            if (i > xPos && i < xPos + width &&
                j > yPos && j < yPos + height &&
                xPos + width <= DISPLAY_WIDTH &&
                yPos + width <= DISPLAY_HEIGHT) {
                    bitArray[i][j] = 0; // 0 = black
                    ESP_LOGI("display_rect:", "painting black for iteration %d %d", i, j);
                }
                
            // kicking watchdog
            if (j % 10 == 0) { 
                vTaskDelay(1); 
            }
        }
    }

    uint8_t hex = 0;
    int bitCounter = 0;
    int hexCounter = 0;

    ESP_LOGI("display_rect:", "converting bits to bytes");
    //convert bits to bytes 
    for (int i = 0; i < DISPLAY_WIDTH; i++) {
        for (int j = 0; j <  DISPLAY_HEIGHT; j++) {
            // gather bytes from rectArray and shift them into hex
            
            if (bitArray[i][j] == 0) {
                hex = (hex << 1);
            }
            else {
                hex = (hex << 1) | 1;
            }
            bitCounter++;
            
            // put hex into byteArray
            if (bitCounter == 8) {
                byteArray[hexCounter++] = hex;
                hex = 0xFF;
                bitCounter = 0;
            }

            // kicking watchdog
            if (j % 10 == 0) { 
                vTaskDelay(1); 
            }
        }
    }

    // // for remaining bits
    // if (bitCounter > 0) {
    //     byteArray[hexCounter] = hex << (8 - bitCounter);
    // }

    int byteArraySize = sizeof(byteArray) / sizeof(byteArray[0]);

    ESP_LOGI("display_rect:", "sending data");
    send_command(0x24);
    for (int i = 0; i < byteArraySize; i++) {
        send_data(byteArray[i]);
    }
    ESP_LOGI("display_rect:", "size - %d", byteArraySize);
}

// uint8_t draw_rect(int xPos, int yPos, int width, int height) {

//     // xPos & yPos is the top-left position for the rectangle

//     int startPos = byteArray[xPos][yPos];
//     int bitCounter = 0;
//     int hexCounter = 0;
//     uint8_t hex = 0x00;

//     unsigned char rectArraySize = (DISPLAY_HEIGHT * DISPLAY_WIDTH) / 8;
//     unsigned char rectArray[rectArraySize];


//     for(int i = xPos; i <= width; i++) {
//         for(int j = yPos; j <= height; j++) {
//             if (bitCounter == 255) {
//                 rectArray[hexCounter] = hex; 

//                 hex = 0x00;
//                 bitCounter = 0;
//                 hexCounter++;
//             }
            
//             if (i >= xPos && i <= width 
//              && j >= yPos && j <= height) {
//                 // byteArray[i][j] = 0; // 0 = black
//                 hex = (hex << 1) | 1; // msb/lsb don't matter since all black
//             }
//             bitCounter++;
            
//             // // end of rectangle
//             // if (i * j > yPos * height) {
//             //     break; 
//             // }
//         }

//     uint8_t arr[1][1];

//     return arr;
//     }

//     send_command(0x24);

//     // fix this shit
//     for(int i = 0; i < 30500 / 8; i++) {
//         if (i >= startPos / 8) {
//             send_data(rectArray[i]);

//             // implement so it stops sending when the end of data is over,
//             // otherwise it will keep sending and overwrite itself
//         }
        
//     }

//     // int arraySize = sizeof(byteArray) / sizeof(byteArray[0]);
// }

void test(void) {
    send_command(0x24);
    send_data(0xF0);
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