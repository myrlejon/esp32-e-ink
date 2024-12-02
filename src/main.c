#include "esp_task_wdt.h"
#include "def.h"

void app_main() {
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = 10000,         // 5-second timeout
        .idle_core_mask = 0,        
        .trigger_panic = true       
    };

    esp_task_wdt_init(&wdt_config);  // Initialize watchdog with 30-second timeout
    esp_task_wdt_add(NULL);       // Add the current task to the watchdog

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
