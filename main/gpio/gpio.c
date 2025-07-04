#include "gpio.h"

void gpio_set_mode(gpio_num_t pin, GPIO_MODE mode) {
    switch (mode)
    {
    case GPIO_OUTPUT:
        gpio_set_direction(pin, GPIO_MODE_OUTPUT);
        gpio_set_pull_mode(pin, GPIO_PULLUP_PULLDOWN);
        break;
    case GPIO_INPUT:
        gpio_set_direction(pin, GPIO_MODE_INPUT);
        gpio_pullup_en(pin);
        break;
    default:
        break;
    }
}

