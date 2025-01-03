#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "number.h"

// config

// write on blank canvas, configure ARRAY_TYPE = 0. write on rendered image, configure ARRAY_TYPE = 1;
#define ARRAY_TYPE 1

// init
void spi_init(void);
void gpio_init(void);
void send_command(uint8_t cmd);
void send_data(uint8_t data);
void display_reset(void);
void wait_until_idle(void);
void display_clear(void);
void display_power_on(void);
void display_deep_sleep(void);
void set_init_configuration(void);
void set_init_code(void);
void load_waveform_lut(void);

// api
void draw(void);
void write_image_txt_to_display(bool invert);
void clear_display();

void draw_rect(int xPos, int yPos, int width, int height);
void draw_large_number(int number, int position);
void draw_small_number(int number, int position, int x_start_pos, int y_start_pos);
void draw();
void draw_pixel(int x, int y, uint8_t color);
void draw_command(bool invert);

// temp
void dht22_read(void);
void record_temp(int current_temp);

// mcu
void mcu_sleep(void);