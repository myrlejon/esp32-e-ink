#include "def.h"

void app_main() {

    //nedanför är 13.1 normal operation flow
    //1. power on
    display_power_on();

    //2. set initial configuration
    spi_init();
    

}
