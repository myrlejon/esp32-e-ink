#include "driver/spi_master.h"
#include "driver/gpio.h"

void spi_init(void);
void gpio_init(void);
void send_command(uint8_t cmd);
void send_data(uint8_t data);
void display_reset(void);
void display_init(void);
void wait_until_idle(void);
void display_image(const uint8_t *image, int length);
void display_clear(void);
void display_power_on(void);
void deep_sleep(void);