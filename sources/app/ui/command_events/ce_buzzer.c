#include "system.h"
#include "ce_buzzer.h"

void ceBuzzer(bool cmdReceived)
{
	if (!cmdReceived)
		return;
	sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
}