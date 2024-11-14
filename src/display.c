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

void spi_init(void) {
        spi_bus_config_t buscfg = {
        .miso_io_num = -1, // not needed as the display is a slave
        .mosi_io_num = PIN_NUM_DIN,
        .sclk_io_num = PIN_NUM_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096
    };
}