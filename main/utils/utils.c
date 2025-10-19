#include "utils.h"
bool utils_valid_pins(uint8_t pin)
{
	if ((pin >= 10 && pin <= 14) || pin == 21) return true;
	return false;
}