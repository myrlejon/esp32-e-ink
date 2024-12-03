#include "esp_task_wdt.h"
#include "def.h"

void app_main() {
    //nedanför är 13.1 normal operation flow
    //1. power on
    display_power_on();

    //2. set initial configuration
    set_init_configuration();

    //3. send Initialization Code
    set_init_code();

    //4. load waveform LUT 
    load_waveform_lut();

    //5. write image
    write_image();

    //6. power off (deep sleep)
    deep_sleep();
}
