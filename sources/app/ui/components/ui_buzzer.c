#include "ui_buzzer.h"
#include "dev/buzzer.h"
#include "app/refrigerator/rf_ram.h"            //APW
#include "app/refrigerator/rf_configs.h"        //APW
#include "app/cabin/cabin_configs.h"            //APW
#include "../components/ui_display.h"

#define _UI_WarningTime (1.2 * 50)
#define _UI_WarningDoorAlertAHTHTime (1 * 50)

void uiBuzzerWarning(const tsUiProcConsts *consts, tsUiProcParams *params, bool isThereWarning)
{
	tsUiRam *uiP = params->ui;

	if (isThereWarning)
	{
		if (!uiP->timers.warningTimer)
		{
			uiP->timers.warningTimer = _UI_WarningTime;
			sysBuzzerCmd(eSysBuzzerPlayWarningMelody);
		}
	}
}
/*** APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1) ***/
void uiBuzzerDoorAlarmAHTH(const tsUiProcConsts *consts, tsUiProcParams *params, bool isThereWarning)
{
	tsUiRam *uiP = params->ui;

	if (isThereWarning)
	{
		if (!uiP->timers.warningTimer)
		{
                    uiP->timers.warningTimer = _UI_WarningDoorAlertAHTHTime;
                        if(refrigerator_R.Values.doorsAlertTimeSelected == 1 &&
                            refrigerator_R.Values.doorsAlertCount < 5)
                        {
                          refrigerator_R.Values.prohibitionAlertDisplay = false;
                          //sysBuzzerCmd(eSysBuzzerPlayDoorsAlertAHTH);
                          refrigerator_R.Values.doorsAlertCount++;
                        }
                        else if(refrigerator_R.Values.doorsAlertTimeSelected == 1 &&
                            refrigerator_R.Values.doorsAlertCount >= 5)
                        {
                          if(!refrigerator_R.Values.prohibitionAlertDisplay)
                            sysBuzzerCmd(eSysBuzzerNone);
                          refrigerator_R.Values.prohibitionAlertDisplay = true;
                          if(refrigerator_R.Values.doorsAlertEcoModeFirstTime)
                          {
                            refrigerator_R.Values.doorsAlertEcoModeFirstTime = false;
                            refrigerator_R.Values.doorsAlertTimeSelected = 0;
                          }
                        }
                        else if(refrigerator_R.Values.doorsAlertTimeSelected == 2 &&
                                  refrigerator_R.Values.doorsAlertCount < 10)
                        {
                          refrigerator_R.Values.prohibitionAlertDisplay = false;
                          //sysBuzzerCmd(eSysBuzzerPlayDoorsAlertAHTH);
                          refrigerator_R.Values.doorsAlertCount++;
                        }
                        else if(refrigerator_R.Values.doorsAlertTimeSelected == 3 ||
                                  refrigerator_R.Values.doorsAlertTimeSelected == 4 ||
                                    refrigerator_R.Values.doorsAlertTimeSelected == 5)
                        {
                          refrigerator_R.Values.prohibitionAlertDisplay = false;
                          refrigerator_R.Values.doorsAlertTimeSelected = 3;
                          //sysBuzzerCmd(eSysBuzzerPlayDoorsAlertAHTH);
                          if(refrigerator_R.Values.doorsAlertCount >= 1)
                            refrigerator_R.Values.doorsAlertCount = refrigerator_R.Values.doorsAlertCount;
                          else
                            refrigerator_R.Values.doorsAlertCount++;
                        }
                        else
                        {
//                          if(!refrigerator_R.Values.prohibitionAlertDisplay)
//                            sysBuzzerCmd(eSysBuzzerNone);
                          refrigerator_R.Values.prohibitionAlertDisplay = true;
                        }
                    if(!refrigerator_R.Values.prohibitionAlertDisplay)
                      refrigerator_R.Values.doorsAlertAndDisplay = true; 
		}
                
              if(params->rf->Values.doorsAlert && !refrigerator_R.Values.prohibitionAlertDisplay)
              {
                if(refrigerator_R.Values.doorsAlertAndDisplay)
                {
                  if(refrigerator_R.Values.doorAlertBuzzerStart)
                    sysBuzzerCmd(eSysBuzzerPlayDoorsAlertAHTH);
                  uiDspAllOn(consts, params);
                  refrigerator_R.Values.doorAlertBuzzerStart = 0;
                }
                else
                {
                  sysBuzzerCmd(eSysBuzzerNone);
                  uiDspAllOff(consts, params);
                  refrigerator_R.Values.doorAlertBuzzerStart = 1;
                }
              }
	}
}
/*** APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1) ***/
// void uiBuzzerCmd(const tsUiProcConsts *consts, teMainBoardCommand command)
// {
// 	devDspSendCommand(consts->dsp, command);
// 	if (consts->rf->Parameters.onBoardBuzzer)
// 	{
// 		switch (command)
// 		{
// 		case eMainBoardCommandBuzzerMOpen:
// 			devIoPut(consts->buzzer, eBuzzerMelodyOpen);
// 			break;

// 		case eMainBoardCommandBuzzerMClose:
// 			devIoPut(consts->buzzer, eBuzzerMelodyClose);
// 			break;

// 		case eMainBoardCommandBuzzerMStart:
// 			devIoPut(consts->buzzer, eBuzzerMelodyStart);
// 			break;

// 		case eMainBoardCommandBuzzerMEnd:
// 			devIoPut(consts->buzzer, eBuzzerMelodyEnd);
// 			break;

// 		case eMainBoardCommandBuzzerMButton:
// 			devIoPut(consts->buzzer, eBuzzerMelodyButton);
// 			break;

// 		case eMainBoardCommandBuzzerMWarning:
// 			devIoPut(consts->buzzer, eBuzzerMelodyWarning);
// 			break;

// 		case eMainBoardCommandBuzzerMDoorsAlert:
// 			devIoPut(consts->buzzer, eBuzzerMelodyWarning);
// 			break;
// 		default:
// 			break;
// 		}
// 	}
// }
