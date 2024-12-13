#include "driver/spi_master.h"
#include "driver/gpio.h"

void spi_init(void);
void gpio_init(void);
void send_command(uint8_t cmd);
void send_data(uint8_t data);
void send_bit_data(uint8_t data);
void display_reset(void);
void wait_until_idle(void);
void display_clear(void);
void display_power_on(void);
void deep_sleep(void);
void set_init_configuration(void);
void set_init_code(void);
void load_waveform_lut(void);
void write_image(void);
void draw(void);
void write_image_to_display(void);
void write_image_txt_to_display(void);
void write_image_task(void *param);
void test(void);
void draw_rect(int xPos, int yPos, int width, int height);
void clear_display();
void draw();
void draw_pixel(int x, int y, uint8_t color);
void draw_command();