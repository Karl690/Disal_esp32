#include "utils.h"
bool utils_valid_pins(uint8_t pin)
{
	if (pin >= 1 && pin <= 15) return true;
	return false;
}