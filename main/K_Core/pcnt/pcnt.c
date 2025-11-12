#include <driver/pulse_cnt.h>
#include <driver/gpio.h>
#include "pcnt.h"

PCNT_INFO pcnt_info = {0, 0, 0, 0, 0, 0};

#define MAX_ADC12 						4096
#define TEMP_FRAC_BITS                  0
#define TEMP_SCALE                      32
#define TEMP_SCALEF                     32.0f
#define MAX_TEMP                        0x7fff  // max positive

pcnt_unit_handle_t PulseCounter_1 = NULL;
pcnt_unit_handle_t PulseCounter_2 = NULL;
int PwmTimerReloadRegister = 0;
int TemperatureFreq;
int Battery_V_Freq;
float RtdVoltage;
float Temperature;
float BatteryVoltage;
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


void pcnt_init(void) {
	//conigure the pulse counter  pcnt
    pcnt_unit_config_t cfg0 = { 
        .low_limit = -1,
        .high_limit = 32000, 
    };
    cfg0.flags.accum_count = true;//keep counting, even after reading
    ESP_ERROR_CHECK(pcnt_new_unit(&cfg0, &PulseCounter_1));//set up counter1

    pcnt_chan_config_t Channel_configuration = { 
        .edge_gpio_num = Counter1_INPUT_PIN,  //assign input pin for pulse counter
        .level_gpio_num = -1,        // Level signal not used for simple pulse counting
	    //do not invert or set defult level, count on rising edge
        .flags = {
            .invert_edge_input = false,
            .invert_level_input = false, 
        }
    };
    pcnt_channel_handle_t chan0;
    ESP_ERROR_CHECK(pcnt_new_channel(PulseCounter_1, &Channel_configuration, &chan0));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(chan0, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(chan0, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));
	//now setup counter 2
	ESP_ERROR_CHECK(pcnt_new_unit(&cfg0, &PulseCounter_2)); //set up counter2
	Channel_configuration.edge_gpio_num = Counter2_INPUT_PIN;//assign the gpio pin to counter2
	//
	pcnt_channel_handle_t chan1;
	ESP_ERROR_CHECK(pcnt_new_channel(PulseCounter_2, &Channel_configuration, &chan1));
	ESP_ERROR_CHECK(pcnt_channel_set_edge_action(chan1, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
	ESP_ERROR_CHECK(pcnt_channel_set_level_action(chan1, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));

    pcnt_glitch_filter_config_t filter_config = { .max_glitch_ns = 100 };//set it to 100ns as we are using 1mhz input pulse
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(PulseCounter_1, &filter_config));
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(PulseCounter_2, &filter_config));

    ESP_ERROR_CHECK(pcnt_unit_enable(PulseCounter_1));
    ESP_ERROR_CHECK(pcnt_unit_enable(PulseCounter_2));

    ESP_ERROR_CHECK(pcnt_unit_clear_count(PulseCounter_1));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(PulseCounter_2));
   
    ESP_ERROR_CHECK(pcnt_unit_start(PulseCounter_1));
    ESP_ERROR_CHECK(pcnt_unit_start(PulseCounter_2));  
}

void EnableCounter() {
    ESP_ERROR_CHECK(pcnt_unit_start(PulseCounter_1));
    ESP_ERROR_CHECK(pcnt_unit_start(PulseCounter_2));
}


void DisableCounter() {
    ESP_ERROR_CHECK(pcnt_unit_stop(PulseCounter_1));
    ESP_ERROR_CHECK(pcnt_unit_stop(PulseCounter_2));
}

// calculate the real data every 1s.
void Convert_Counter1_To_Temperature() {
	//at this point the counter has been read and converted to a voltage "RtdVoltage"
   // RtdVoltage = TemperatureFreq * systemconfig.pcnt.rtd_scale; old method
    Temperature = pcnt_convert_temperature(RtdTable_1K, RtdVoltage);//use lookup table to convert voltage to temperature
    pcnt_info.temperature = Temperature;

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
	
//test pulse 
	systemconfig.pcnt.duty_test++; //count up
	if (systemconfig.pcnt.duty_test & 0x0001)
	{
		gpio_set_level(ControlOutput_PIN, 1); //disable the heater until code is stable
	}
	else
	{
		gpio_set_level(ControlOutput_PIN, 0); //disable the heater until code is stable	
	}
//	if (systemconfig.pcnt.duty_test == 0)
//	{
//		//normal duty cycle process
//		PwmTimerReloadRegister = pcnt_info.duty; //setthe reload timer		
//	}
//	else
//	{//first do a range check , duty must be between 0 and 100%
//		if (systemconfig.pcnt.duty_test > 100)systemconfig.pcnt.duty_test = 100;
//		if (systemconfig.pcnt.duty_test <0)systemconfig.pcnt.duty_test = 0;
//		PwmTimerReloadRegister = systemconfig.pcnt.duty_test;
//	}
}

void Scale_BatteryVoltage() {
    BatteryVoltage = Battery_V_Freq * systemconfig.pcnt.battery_scale;
    pcnt_info.bat_volt = BatteryVoltage;

     
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

void Read_Counters() {
	//reads both counter1 and 2, then resets counters to 0, used in 100hz loop
	//so we are actually getting frequency in 100 hz resolution
    ESP_ERROR_CHECK(pcnt_unit_get_count(PulseCounter_1, &pcnt_info.count01));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(PulseCounter_1));


    ESP_ERROR_CHECK(pcnt_unit_get_count(PulseCounter_2, &pcnt_info.count02));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(PulseCounter_2));
    Battery_V_Freq= pcnt_info.count02;
	//now process the variables into voltage
	TemperatureFreq = pcnt_info.count01;
	RtdVoltage = TemperatureFreq / systemconfig.pcnt.rtd_scale;
	pcnt_info.rtd_volt = RtdVoltage; //update global
	//battery voltage next
	BatteryVoltage = Battery_V_Freq / systemconfig.pcnt.battery_scale;
	pcnt_info.bat_volt = BatteryVoltage;//update global variable
	
}
