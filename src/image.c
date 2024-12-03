#include "def.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <image.h>
#include "esp_task_wdt.h"

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

void write_image_task(void *param) {
    // Register this task to the Task Watchdog
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL)); // NULL refers to the current task

    send_command(0x24);
    // int imageArraySize = sizeof(image_array) / sizeof(image_array[0]);

    for (int i = 0; i < 2000; i++) {
        
        if (i < 1000) {
            send_data(0x00);
        }

        if (i % 500 == 0) {
            ESP_LOGI("image info:", "kicking watchdog!");
            esp_task_wdt_reset();
        }
    }

    ESP_ERROR_CHECK(esp_task_wdt_delete(NULL));
    ESP_LOGI("write_image_task", "Image writing complete");
    vTaskDelete(NULL);
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