#include "epd_driver.h"
#include "epd_internals.h"
#include "epd_board_specific.h"

#define EPD_WIDTH 250
#define EPD_HEIGHT 122


// #define CONFIG_EPD_DISPLAY_TYPE_CUSTOM


void app_main() {
    EpdRect rect;

    uint8_t framebuffer[EPD_WIDTH * EPD_HEIGHT / 8] = {0};

    rect.width = 10;
    rect.height = 10;
    rect.x = 10;
    rect.y = 10;

    epd_init(EPD_OPTIONS_DEFAULT);
    epd_clear();
    epd_set_rotation(1);
    epd_draw_rect(rect, PREVIOUSLY_WHITE, framebuffer);
}
