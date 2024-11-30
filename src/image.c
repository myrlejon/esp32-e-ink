#include "def.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

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

void write_image_to_display(void) {
    const char *file_path = "C:/Users/nisse/Documents/PlatformIO/Projects/esp32-e-ink/src/dithered_image.bmp";
    FILE *bmp_file = fopen(file_path, "rb");
    if (!bmp_file) {
        printf("Failed to open BMP file\n");
        return;
    }

    // Skip BMP header (54 bytes for a standard BMP)
    fseek(bmp_file, 54, SEEK_SET);

    // Prepare to read pixel data
    uint8_t row_data[DISPLAY_WIDTH / 8]; // Each byte = 8 pixels

    send_command(0x24); // Write RAM command

    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        fread(row_data, sizeof(uint8_t), DISPLAY_WIDTH / 8, bmp_file);

        // Send row data to the display
        for (int x = 0; x < DISPLAY_WIDTH / 8; x++) {
            send_data(row_data[x]);
        }
    }
    fclose(bmp_file);
}