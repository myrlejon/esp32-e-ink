#include "def.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <image.h>

#define DISPLAY_WIDTH 250
#define DISPLAY_HEIGHT 122

unsigned char byteArray[30500];

void draw() {

    send_command(0x24);
    
    int arraySize = sizeof(byteArray) / sizeof(byteArray[0]);

    for (int i = 0; i < arraySize / 8; i++) {
        
        if (i % 2 == 0) {
            byteArray[i] = 0xFF;
        }
        if (i % 3 == 0) {
            byteArray[i] = 0x00;
        }
        send_data(byteArray[i]);
    }
}

void write_image_txt_to_display(void) {
    send_command(0x24);
    int imageArraySize = sizeof(image_array) / sizeof(image_array[0]);

    ESP_LOGI("image info:", "array size: %d", imageArraySize);

    // TODO - fix watchdog timer

    for (int i = 0; i < 4996; i++) {
        if (i % 100 == 0) {
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        if (i % 2 == 0) {
            ESP_LOGI("info:", "iteration: %d", i);
            send_data(0x00);
        }
        else {
            ESP_LOGI("info:", "iteration: %d", i);
            send_data(0xFF);    
        }
        // send_data(image_array[i]);
    }
}

// void write_image_to_display(void) {
//     const char *file_path = "dithered_image.bmp";
//     FILE *bmp_file = fopen(file_path, "rb");
//     if (!bmp_file) {
//         printf("Failed to open BMP file\n");
//         return;
//     }

//     fseek(bmp_file, 54, SEEK_SET);
//     uint8_t row_data[DISPLAY_WIDTH / 8];
//     send_command(0x24);

//     for (int y = 0; y < DISPLAY_HEIGHT; y++) {
//         fread(row_data, sizeof(uint8_t), DISPLAY_WIDTH / 8, bmp_file);

//         // Send row data to the display
//         for (int x = 0; x < DISPLAY_WIDTH / 8; x++) {
//             send_data(row_data[x]);
//         }
//     }
//     fclose(bmp_file);
// }