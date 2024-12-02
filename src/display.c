#include "def.h"
#include "esp_log.h"

// flexible flat cable pins
// VCC = VCC
// GND = GND

const uint32_t TOTAL_PIXELS = 30500;

#define PIN_NUM_KEY 12 //KEY
#define PIN_NUM_SCK 13 //SCK or CLK clock input
#define PIN_NUM_DIN 14 //DIN or MOSI (master out slave in)
#define PIN_NUM_CS 15 //CS
#define PIN_NUM_BUSY 25 //BUSY
#define PIN_NUM_RST 26 //RST
#define PIN_NUM_DC 27 //DC - 0 = command mode | 1 = data mode
#define PIN_NUM_ADC 36 //ADC

spi_device_handle_t spi;

void spi_init(void) {
    ESP_LOGI("init", "starting SPI init...");
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
    ESP_LOGI("init", "SPI done");
}

void gpio_init(void) {
    ESP_LOGI("init", "init GPIO...");
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_BUSY, GPIO_MODE_INPUT);
    ESP_LOGI("init", "GPIO done");
}

// send_command() och send_data() kan bli en metod med level som parameter
void send_command(uint8_t cmd) {
    gpio_set_level(PIN_NUM_DC, 0); // command mode
    spi_transaction_t t = {
        .length = 8,              // command is 8 bits
        .tx_buffer = &cmd
    };
    spi_device_transmit(spi, &t);
}

void send_data(uint8_t data) {
    gpio_set_level(PIN_NUM_DC, 1); // data mode
    spi_transaction_t t = {
        .length = 8,              // data is 8 bits
        .tx_buffer = &data
    };
    spi_device_transmit(spi, &t);
}

void read_data(uint8_t *buffer, size_t length) {
    // Set to data mode
    gpio_set_level(PIN_NUM_DC, 1);

    spi_transaction_t t = {
        .length = length * 8,    // Read 'length' bytes
        .rx_buffer = buffer     // Buffer to store received data
    };

    esp_err_t ret = spi_device_transmit(spi, &t);
    if (ret != ESP_OK) {
        ESP_LOGE("SPI", "Failed to read data: %s", esp_err_to_name(ret));
    }
}

void display_reset(void) {
    ESP_LOGI("display reset", "resetting display...");
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    ESP_LOGI("display reset", "done");
}

void display_clear(void) {
    send_command(0x24); // RAM write command
    for (int i = 0; i < TOTAL_PIXELS; i++) {
        send_data(0x00); // all black
    }
    send_command(0x24);
    for (int i = 0; i < TOTAL_PIXELS; i++) {
        send_data(0xFF); // all white
    }

    send_command(0x22); // Display update control
    send_data(0xF7);    // Full update mode
    send_command(0x20); // Trigger refresh
    wait_until_idle();
}

void wait_until_idle(void) {
    while (gpio_get_level(PIN_NUM_BUSY) == 1) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}


// 13.1 - 1. supply VCI > wait 10ms
void display_power_on(void) {
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

// 13.1 - 2. set initial configuration
void set_init_configuration(void) {
    ESP_LOGI("init", "starting initial configuration");
    gpio_init();
    spi_init();
    display_reset();
    send_command(0x12); //SW reset
    wait_until_idle();
    vTaskDelay(10 / portTICK_PERIOD_MS);
    ESP_LOGI("init", "init done");
}

// 13.1 - 3. set initialization code
void set_init_code(void) {
    ESP_LOGI("init", "starting 3. set init code...");
    // set gate driver output by command 0x01
    send_command(0x01);
    send_data(0x27);
    send_data(0x01);
    send_data(0x00);
    // booster soft start
    send_command(0x0C);
    send_data(0x8B);
    send_data(0x9C);
    send_data(0x96);
    send_data(0x0F);
    // data entry mode setting
    send_command(0x11);
    send_data(0x03);
    // set display RAM size
    send_command(0x44); // X-axis
    send_data(0x00);
    send_data(0x15);
    send_command(0x45); // Y-axis
    send_data(0x00);
    send_data(0x00);
    send_data(0x27);
    send_data(0x01);
    // set panel border
    send_command(0x3C);
    send_data(0xC0);
    // gate driving voltage
    send_command(0x03);
    send_data(0x00);
    // source driving voltage
    send_command(0x04);
    send_data(0x97); // 0x97 = 3.3V | 0xA8 = 5V
    send_data(0x00);
    send_data(0x00);
    ESP_LOGI("init", "done");
}

// 13.1 - 4. load waveform LUT
void load_waveform_lut(void) {
    ESP_LOGI("load_waveform_lut", "starting...");
    send_command(0x18);  // temperature sense command
    send_data(0x80); // internal temp sensor

    // send_command(0x21);
    // send_data(0x00);

    send_command(0x22);  // load waveform from OTP command
    send_data(0xC7);
    send_command(0x20);  // trigger the load process
    
    // wait for BUSY pin
    wait_until_idle();
    ESP_LOGI("load_waveform_lut", "done");
}

// TODO - add parameter for sending in data to write RAM (0x24)
// 13.1 - 5. write image and drive display panel 
void write_image() {
    ESP_LOGI("write_image", "starting...");
    // clear display
    display_clear();
    
    // write image data in RAM
    send_command(0x4E); // X adress
    send_data(0x00);
    send_command(0x4F); // Y adress
    send_data(0x00); // low byte - 0-255
    send_data(0x00); // high byte - for Y > 255
    
    wait_until_idle();

    write_image_txt_to_display();
    //write_image_to_display();
    // draw();

    wait_until_idle();

    send_command(0x22);  // load waveform from OTP command
    send_data(0xF7); // 0xF7 does 0xC7 but with temp sensor
    send_command(0x20);  // trigger the load process

    ESP_LOGI("write_image", "done");
}

// 13.1 - 6. power off 
void deep_sleep(void) {
    ESP_LOGI("deep sleep", "goodnight");
    send_command(0x10);
    send_data(0x01); // deep sleep mode 1 (se commands 7)
}

