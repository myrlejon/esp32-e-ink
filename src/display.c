#include "def.h"


// flexible flat cable pins
// VCC = VCC
// GND = GND

#define PIN_NUM_KEY 12 //KEY
#define PIN_NUM_SCK 13 //SCK or CLK clock input
#define PIN_NUM_DIN 14 //DIN or MOSI (master out slave in)
#define PIN_NUM_CS 15 //CS
#define PIN_NUM_BUSY 25 //BUSY
#define PIN_NUM_RST 26 //RST
#define PIN_NUM_DC 27 //DC
#define PIN_NUM_ADC 36 //ADC

spi_device_handle_t spi;

void spi_init(void) {
    spi_bus_config_t buscfg = {
        .miso_io_num = -1, // not needed as the display is a slave
        .mosi_io_num = PIN_NUM_DIN,
        .sclk_io_num = PIN_NUM_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096
    };
    spi_bus_initialize(HSPI_HOST, &buscfg, 1);

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10 * 1000 * 1000, // 10 MHz
        .mode = 0,                          // SPI mode 0
        .spics_io_num = PIN_NUM_CS,         // CS pin
        .queue_size = 7,                    // Queue size
    };
    spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
}

void gpio_init(void) {
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_BUSY, GPIO_MODE_INPUT);
}

void send_command(uint8_t cmd) {
    gpio_set_level(PIN_NUM_DC, 0); // command mode
    spi_transaction_t t = {
        .length = 8,              // command is 8 bits
        .tx_buffer = &cmd
    };
    spi_device_transmit(spi, &t);
}

// TODO!

void display_reset(void) {
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

void display_init(void) {
    display_reset();
    send_command(0x01);  // POWER SETTING
    send_data(0x03);     // Example settings; check your model's initialization sequence
    send_data(0x00);
    send_data(0x2b);
    send_data(0x2b);
    send_command(0x06);  // BOOSTER SOFT START
    send_data(0x17);
    send_data(0x17);
    send_data(0x17);
    // Continue with additional commands as per datasheet
}

void wait_until_idle(void) {
    while (gpio_get_level(PIN_NUM_BUSY) == 1) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void display_image(const uint8_t *image, int length) {
    send_command(0x24); // Write RAM command
    for (int i = 0; i < length; i++) {
        send_data(image[i]);
    }
    send_command(0x22); // Display update control
    send_data(0xF7);    // Mode for full update
    send_command(0x20); // Trigger display refresh
    wait_until_idle();
}