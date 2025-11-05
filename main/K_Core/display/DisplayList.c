#include "DisplayList.h"
#include "taskmanager.h"
#include "pcnt/pcnt.h"
#include "RevisionHistory.h"

DisplayVariableInfo LcdDiagVarsTable[] = {
	{ (void*)&HeartBeat, "HB", FUNC_INT32, COLOR_WHITE, COLOR_RED, 0, 0, NULL },
	{ &pcnt_info.count01, "CNT#1", FUNC_INT32, COLOR_WHITE, COLOR_RED, 0, 0, NULL },
	{ &pcnt_info.count02, "CNT#1", FUNC_INT32, COLOR_WHITE, COLOR_RED, 0, 0, NULL },
	{ &pcnt_info.temperature, "TEMP", FUNC_FLOAT, COLOR_WHITE, COLOR_RED, 0, 0, NULL },
	{ &pcnt_info.freq, "Freq", FUNC_INT32, COLOR_WHITE, COLOR_RED, 0, 0, NULL },
	{ &pcnt_info.duty, "Duty", FUNC_FLOAT, COLOR_WHITE, COLOR_RED, 0, 0, NULL },
	{ &pcnt_info.rtd_volt, "Rtd_volt", FUNC_FLOAT, COLOR_WHITE, COLOR_RED, 0, 0, NULL },
	{ &pcnt_info.bat_volt, "Bat_volt", FUNC_FLOAT, COLOR_WHITE, COLOR_RED, 0, 0, NULL },
};

const int LcdDiagVarsSize = sizeof(LcdDiagVarsTable) / sizeof(DisplayVariableInfo);
