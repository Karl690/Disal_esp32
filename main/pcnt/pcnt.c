#include <driver/pcnt.h>
#include <driver/gpio.h>
#include "pcnt.h"

PCNT_INFO pcnt_info = {0, 0, 0, 0, 0, 0};

#define MAX_ADC12 						4096
#define TEMP_FRAC_BITS                  0
#define TEMP_SCALE                      32
#define TEMP_SCALEF                     32.0f
#define MAX_TEMP                        0x7fff  // max positive
AdcTableStruct const RtdTable_1K[] __attribute__((aligned(4))) =
{ // 1K RTD -- based on datasheet
		// 20 entries; 4 bytes each; 80 bytes total
		// (uint16_t)adcVal, (uint16_t)(tempC * 32)    (MUST be in order with increasing adcVals)
	{ 0, (-242 << TEMP_FRAC_BITS) }, // extrapolated to get -242 as 0 ohms (yielding 0 adc value with 2K divider)
	{ 347, (-200 << TEMP_FRAC_BITS) }, // 1K mfg table starts at -200
	{ 678, (-150 << TEMP_FRAC_BITS) },
	{ 948, (-100 << TEMP_FRAC_BITS) },
	{ 1173, (-50 << TEMP_FRAC_BITS) },
	{ 1365, (0 << TEMP_FRAC_BITS) },
	{ 1531, (50 << TEMP_FRAC_BITS) },
	{ 1676, (100 << TEMP_FRAC_BITS) },
	{ 1803, (150 << TEMP_FRAC_BITS) },
	{ 1916, (200 << TEMP_FRAC_BITS) },
	{ 2017, (250 << TEMP_FRAC_BITS) },
	{ 2107, (300 << TEMP_FRAC_BITS) },
	{ 2189, (350 << TEMP_FRAC_BITS) },
	{ 2263, (400 << TEMP_FRAC_BITS) },
	{ 2330, (450 << TEMP_FRAC_BITS) },
	{ 2392, (500 << TEMP_FRAC_BITS) },
	{ 2448, (550 << TEMP_FRAC_BITS) },
	{ 2500, (600 << TEMP_FRAC_BITS) }, // 1K mfg tables ends at 600
	{ 2809, (999 << TEMP_FRAC_BITS) }, // extrapolated to get 999 as 4370 ohms (yielding 2809 adc value with 2K divider)
	{ MAX_ADC12, MAX_TEMP },
	// in reality, to get to MAX_ADC, would take about 10 billion degrees.
};

float pcnt_convert_temperature(const AdcTableStruct* adcTable, float voltage);

void pcnt_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PCNT_INPUT_PIN_01) | (1ULL << PCNT_INPUT_PIN_02),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    gpio_config_t ctrl_conf = {
        .pin_bit_mask = (1ULL << PCNT_CTRL_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    esp_err_t err = gpio_config(&ctrl_conf);
    gpio_set_level((gpio_num_t)PCNT_CTRL_PIN, 1); // turn on

    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = PCNT_INPUT_PIN_01,  // plus input pin
        .ctrl_gpio_num = PCNT_PIN_NOT_USED,  // control pin (not used)
        .lctrl_mode = PCNT_MODE_KEEP,      // low level control mode
        .hctrl_mode = PCNT_MODE_KEEP,      // high level control mode
        .pos_mode = PCNT_COUNT_INC,        //  increase count on up edge
        .neg_mode = PCNT_COUNT_DIS,        // increase count on down edge
        .counter_h_lim = 32767,            // max count value
        .unit = PCNT_UNIT_0,
        .channel = PCNT_CHANNEL_0
    };

    ESP_ERROR_CHECK(pcnt_unit_config(&pcnt_config));
    pcnt_config.pulse_gpio_num = PCNT_INPUT_PIN_02;  // plus input pin
    
    ESP_ERROR_CHECK(pcnt_unit_config(&pcnt_config));
    
    ESP_ERROR_CHECK(pcnt_counter_pause(PCNT_UNIT_0));
    ESP_ERROR_CHECK(pcnt_counter_clear(PCNT_UNIT_0));

    ESP_ERROR_CHECK(pcnt_counter_pause(PCNT_UNIT_1));
    ESP_ERROR_CHECK(pcnt_counter_clear(PCNT_UNIT_1));
    // ESP_ERROR_CHECK(pcnt_counter_resume(PCNT_UNIT));
}

void pcnt_start() {
    ESP_ERROR_CHECK(pcnt_counter_resume(PCNT_UNIT_0));
    ESP_ERROR_CHECK(pcnt_counter_resume(PCNT_UNIT_1));
}

void pcnt_stop() {
    ESP_ERROR_CHECK(pcnt_counter_pause(PCNT_UNIT_0));
    ESP_ERROR_CHECK(pcnt_counter_pause(PCNT_UNIT_0));
}

void pcnt_get_value() {
    if (systemconfig.pcnt.enabled == 0) return;
    ESP_ERROR_CHECK(pcnt_get_counter_value(PCNT_UNIT_0, &pcnt_info.count01));
    ESP_ERROR_CHECK(pcnt_get_counter_value(PCNT_UNIT_1, &pcnt_info.count02));
    pcnt_info.bat_volt = (float)pcnt_info.count01 * systemconfig.pcnt.battery_scale;
    pcnt_info.rtd_volt = (float)pcnt_info.count02 * systemconfig.pcnt.rtd_scale;
    pcnt_info.temperature = pcnt_convert_temperature(RtdTable_1K, pcnt_info.rtd_volt);
    float deltaTemp = systemconfig.pcnt.programmed_temperature - pcnt_info.temperature;
    if (deltaTemp != 0) {
        pcnt_info.duty = deltaTemp * systemconfig.pcnt.temp_scale;
        gpio_set_level((gpio_num_t)systemconfig.pcnt.ctrl_pin, 1); // turn on
    } else {
        pcnt_info.duty = 0;
        gpio_set_level((gpio_num_t)systemconfig.pcnt.ctrl_pin, 0); // turn off
    }
    
    ESP_ERROR_CHECK(pcnt_counter_clear(PCNT_UNIT_0));
    ESP_ERROR_CHECK(pcnt_counter_clear(PCNT_UNIT_1));
}


float pcnt_convert_temperature( const AdcTableStruct* adcTable, float voltage) {
    uint8_t leftIndex = 0, rightIndex = 0;

	while (adcTable[rightIndex].adcRaw != MAX_ADC12)
	{
		if (voltage <= adcTable[rightIndex].adcRaw) {

			break;
		}
		rightIndex++;
	}
	if (rightIndex == 0) return adcTable[rightIndex].adcRaw;
	else if (rightIndex > 0) leftIndex = rightIndex - 1;

	float a = (adcTable[rightIndex].value - adcTable[leftIndex].value) / (float)(adcTable[rightIndex].adcRaw - adcTable[leftIndex].adcRaw);
	float y = a * (voltage - adcTable[leftIndex].adcRaw) + adcTable[leftIndex].value;
    return y * PCNT_TEMP_SCAL_VALUE;
}