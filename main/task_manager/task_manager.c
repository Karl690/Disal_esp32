#include "task_manager.h"
#include "pcnt/pcnt.h"

volatile uint16_t SliceCnt = 0; // current slice being processed
volatile uint16_t SliceOffset = 0;
volatile uint32_t HeartBeat;
esp_timer_handle_t systickTimer;

const PFUNC F1000HZ[NUM_1000HZ] =
{
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
};

const PFUNC F100HZ[NUM_100HZ] =
{
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
};

const PFUNC F10HZ[NUM_10HZ] =
{
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
};

const PFUNC F1HZ[NUM_1HZ] =
{
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	Spare,
	pcnt_get_value,
	BlinkHeartBeat,
};
/*
 * to make the slice scedular more effecient, we are going to use a binary counter, not base 10
 *
 */
//void TaskManager()
void func_SystickCallback(void* arg)
{
	SliceCnt++;
	SliceOffset = SliceCnt & 0x0007; //precalculate the slice index into the jump table
	if (SliceOffset)
	{
		F1000HZ[SliceOffset](); //run the task
		
		return;//toggle pin so we can see on Oscillosclope and exit
	}
	if (SliceCnt & 0x0038)
	{
		SliceOffset = (SliceCnt >> 3) & 0x0007;
		F100HZ[SliceOffset]();
		SliceOffset += 8;
		return;//toggle pin so we can see on Oscillosclope and exit
	}

	if (SliceCnt & 0x01C0)
	{
		SliceOffset = (SliceCnt >> 6)  & 0x0007;
		F10HZ[SliceOffset]();
		SliceOffset += 16;
		return;//toggle pin so we can see on Oscillosclope and exit
	}

	SliceOffset = (SliceCnt >> 9)  & 0x0007;
	F1HZ[SliceOffset]();
	SliceOffset += 24;
	
	return;//toggle pin so we can see on Oscillosclope and exit

}

void Spare(void)
{
	// placeholder call for empty slice
}
void BlinkHeartBeat(void)
{	
	HeartBeat++;
}


void taskamanger_task(void* arg)
{
	
}

void task_manager_init()
{
	esp_timer_create_args_t systickTimerArgs = {
		.callback = func_SystickCallback,
		.arg = NULL,
		.dispatch_method = ESP_TIMER_TASK,
		.name = "systick timer"
	};
	esp_timer_create(&systickTimerArgs, &systickTimer); // Create the timer
	esp_timer_start_periodic(systickTimer, 244); // Start the timer in automatic reload mode with a period of 125 microseconds
}