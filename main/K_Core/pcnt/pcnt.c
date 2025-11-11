#include <driver/pulse_cnt.h>
#include <driver/gpio.h>
#include "pcnt.h"

PCNT_INFO pcnt_info = {0, 0, 0, 0, 0, 0};

#define MAX_ADC12 						4096
#define TEMP_FRAC_BITS                  0
#define TEMP_SCALE                      32
#define TEMP_SCALEF                     32.0f
#define MAX_TEMP                        0x7fff  // max positive

pcnt_unit_handle_t pcnt_unit_0 = NULL;
pcnt_unit_handle_t pcnt_unit_1 = NULL;
int PwmTimerReloadRegister = 0;

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
    pcnt_unit_config_t cfg0 = { 
        .low_limit = -32767,
        .high_limit = 32767, 
    };
    cfg0.flags.accum_count = false;
    ESP_ERROR_CHECK(pcnt_new_unit(&cfg0, &pcnt_unit_0));

    pcnt_chan_config_t chan0_cfg = { 
        .edge_gpio_num = Counter1_INPUT_PIN,
        .level_gpio_num = -1,        // Level signal not used for simple pulse counting
        .flags = {
            .invert_edge_input = false,
            .invert_level_input = false, 
        }
    };
    pcnt_channel_handle_t chan0;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit_0, &chan0_cfg, &chan0));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(chan0, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(chan0, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));

    
    pcnt_unit_config_t cfg1 = { 
        .low_limit = -32767,
        .high_limit = 32767,
    };
    cfg1.flags.accum_count = true;
    ESP_ERROR_CHECK(pcnt_new_unit(&cfg1, &pcnt_unit_1));

    pcnt_chan_config_t chan1_cfg = { 
        .edge_gpio_num = Counter2_INPUT_PIN,
        .level_gpio_num = -1,        // Level signal not used for simple pulse counting
        .flags = {
            .invert_edge_input = false,
            .invert_level_input = false,
        }
    };
    pcnt_channel_handle_t chan1;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit_1, &chan1_cfg, &chan1));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(chan1, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(chan1, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));
    
    pcnt_glitch_filter_config_t filter_config = { .max_glitch_ns = 1000 };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit_0, &filter_config));
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit_1, &filter_config));

    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit_0));
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit_1));

    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit_0));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit_1));
   
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit_0));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit_1));
}

void EnableCounter() {
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit_0));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit_1));
}


void DisableCounter() {
    ESP_ERROR_CHECK(pcnt_unit_stop(pcnt_unit_0));
    ESP_ERROR_CHECK(pcnt_unit_stop(pcnt_unit_1));
}

// calculate the real data every 1s.
void Convert_Counter1_Temperature() {
    pcnt_info.temperature = pcnt_convert_temperature(RtdTable_1K, pcnt_info.rtd_volt);
    float deltaTemp = systemconfig.pcnt.programmed_temperature - pcnt_info.temperature;
    if (deltaTemp != 0) {
        if (systemconfig.pcnt.duty_test > 0) pcnt_info.duty = deltaTemp * systemconfig.pcnt.temp_scale;
        else pcnt_info.duty = 0;
        //gpio_set_level((gpio_num_t)systemconfig.pcnt.ctrl_pin, 1); // turn on
    } else {
        pcnt_info.duty = 0;
       // gpio_set_level((gpio_num_t)systemconfig.pcnt.ctrl_pin, 0); // turn off
    }
}

void SetPwmOutput()
{
	if (systemconfig.pcnt.duty_test == 0)
	{
		//normal duty cycle process
		PwmTimerReloadRegister = pcnt_info.duty; //setthe reload timer		
	}
	else
	{//first do a range check , duty must be between 0 and 100%
		if (systemconfig.pcnt.duty_test > 100)systemconfig.pcnt.duty_test = 100;
		if (systemconfig.pcnt.duty_test <0)systemconfig.pcnt.duty_test = 0;
		PwmTimerReloadRegister = systemconfig.pcnt.duty_test;
	}
}

void Convert_Counter2_BatteryVoltage() {
    pcnt_info.bat_volt = (float)pcnt_info.count01 * systemconfig.pcnt.battery_scale;
    pcnt_info.rtd_volt = (float)pcnt_info.count02 * systemconfig.pcnt.rtd_scale;
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

void ReadCount1() {
    if (systemconfig.pcnt.enabled == 0) return;
    ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit_0, &pcnt_info.count01));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit_0));
    // ESP_LOGI(TAG, "Read Count1 : %d", pcnt_info.count01);
}

void ReadCount2() {
    if (systemconfig.pcnt.enabled == 0) return;
    ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit_1, &pcnt_info.count02));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit_1));
    // ESP_LOGI(TAG, "Read Count2 : %d", pcnt_info.count02);
}
