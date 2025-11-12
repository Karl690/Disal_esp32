#include "k_core.h"
#include "pcnt/pcnt.h"
#include "taskmanager.h"
#include "gpio/gpio.h"
void K_Core_Main() {
    gpio_init();
	// adc_init();
	// serial_init();
    pcnt_init();
    taskmanager_init();
}