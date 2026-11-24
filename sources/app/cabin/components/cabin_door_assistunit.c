#include "cabin_door_assistunit.h"

#define _Door_Assist_Delay              7        //Sec
#define _Door_Assist_first_Duty         20        //% Duty
#define _Door_Assist_Operate_FWD_Duty   100       //% Duty
#define _Door_Assist_Operate_RVS_Duty   90       //% Duty
#define _Door_Assist_last_Duty         60        //% Duty
#define _Door_Assist_Wait_OUT_IN_Time   25        //Timer(s) * Base Timer 20ms(1s)
#define _Door_Assist_OverCurrentLimit   -260
#define _Door_Assist_CurrentLowerBound  -600

// Soft Open: linear ramp
#define _Door_Assist_FWD_Ramp_Step_x10  10       // 1.0% / tick(20ms) => 15%/s  (20%->80% ~4 s)
// Init (homing) : run IN (case 4..) at low duty, stop when motor current is high
#define _Door_Assist_Init_Duty          60      //% Duty (must be above gear dead zone)
#define _Door_Assist_Init_Blank_Ticks   15      // tick(20ms) = 300 ms, ignore inrush current
#define _Door_Assist_Init_I_Limit       50     // count (abs) *** tune from real measurement ***
#define _Door_Assist_Init_I_Max_Valid   -600     // count (abs) above this = spike, ignore
#define _Door_Assist_Init_Detect_Ticks  3       // tick(20ms) = 60 ms continuous
#define _Door_Assist_Init_Timeout       250     // tick(20ms) = 5 s (kept <= 250 like other timers)

// Set to 1 to enable life-cycle test (auto re-arms request after each cooldown).
#define _Door_Assist_LifeTestEnable     0


signed short DEBUG_valueInCExt;
static unsigned short dutyX10;                  // duty * 10 (only ramp OUT)

static unsigned char  initDone = 0;             // init finished (or failed) once after power up
static unsigned char  initMode = 0;             // 1 = running init
static unsigned char  initCnt  = 0;             // continuous high current counter
static unsigned short initTick = 0;             // tick since init start

#ifdef ALPS_H50

void cabinDoorAssistUnit20MsTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if(cabinP->values.doorAssist.procTimer)
      cabinP->values.doorAssist.procTimer--;

    if(cabinC->components.doorAssistMotor.type != _DoorAssistMotorType_NONE)
    {
      cabinDoorAssistUnitTask(consts, params);
    }
}

void cabinDoorAssistUnitT1sec(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    if(cabinP->values.doorAssist.delayTimer)
      cabinP->values.doorAssist.delayTimer--;
    else
    {
      cabinP->values.doorAssist.operationFlag = false;
    }

#if _Door_Assist_LifeTestEnable
    if(!params->rf->Cabin[0].values.doorAssist.operationFlag)
    {
      params->rf->Cabin[0].values.doorAssist.request = true;
    }
#endif
}


void cabinDoorAssistUnitTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    // Start init once after power up (only when no request is running)
    if(!initDone && !initMode && !cabinP->values.doorAssist.request)
    {
      initMode = 1;
      initCnt  = 0;
      initTick = 0;
      cabinP->values.doorAssist.proc = 4;         //Pull IN (low duty, see case 4)
      cabinP->values.doorAssist.request = true;
    }

    if(cabinP->values.doorAssist.request)
    {
      cabinP->values.doorAssist.operationFlag = true;

      DEBUG_valueInCExt = params->rf->Cabin[0].components.doorAssistMotorOverCurrentSensor.valueInCExt;

      // Force Pull-IN on overload only while still in OUT process (proc 0..3).
      // Once we are already in IN process or in WAIT, do not re-init or we get
      // stuck restarting case 4 every cycle while the motor is loaded.
      if((cabinP->values.doorAssist.proc < 4) &&
         (cabinP->components.doorAssistMotorOverCurrentSensor.valueInCExt < _Door_Assist_OverCurrentLimit) &&
         (cabinP->components.doorAssistMotorOverCurrentSensor.valueInCExt > _Door_Assist_CurrentLowerBound))
        cabinP->values.doorAssist.proc = 4;       //Pull IN

      // Init only: stop when current stays high (reached end stop)
      if(initMode &&
         (cabinP->values.doorAssist.proc >= 4) &&
         (cabinP->values.doorAssist.proc <= 6))
      {
        if(initTick < 0xFFFF) initTick++;

        if(initTick > _Door_Assist_Init_Blank_Ticks)
        {
          if((cabinP->components.doorAssistMotorOverCurrentSensor.valueInCExt < _Door_Assist_Init_I_Limit) && (cabinP->components.doorAssistMotorOverCurrentSensor.valueInCExt > _Door_Assist_Init_I_Max_Valid))
          {
            if(++initCnt >= _Door_Assist_Init_Detect_Ticks)
              cabinP->values.doorAssist.proc = 8;   // end stop reached: stop (case 8)
          }
          else
          {
            initCnt = 0;
          }
        }
      }
      switch(cabinP->values.doorAssist.proc)
      {
        /*Out Process*/
      case 0:
        cabinP->components.doorAssistMotor.value = _DoorAssistMotor_FWD;
        dutyX10 = _Door_Assist_first_Duty * 10;
        cabinP->components.doorAssistOut.speedOrPWM = _Door_Assist_first_Duty;

        cabinP->values.doorAssist.procTimer = 250; // TIMER 5s SET (soft open ramp ~4 s)
        cabinP->values.doorAssist.proc = 1;
        break;

      case 1:
        if(cabinP->components.doorAssistInputSwitch.value == _SwitchValue_OPEN)
        {
          cabinP->values.doorAssist.procTimer = 250; // TIMER 5s SET
          cabinP->values.doorAssist.proc = 2;
        }
        else
        {
          if(!cabinP->values.doorAssist.procTimer)
          {
            cabinP->values.doorAssist.proc = 100;
          }

          // Soft Open: linear ramp 
          if(dutyX10 < _Door_Assist_Operate_FWD_Duty * 10)
          {
            dutyX10 += _Door_Assist_FWD_Ramp_Step_x10;
            if(dutyX10 > _Door_Assist_Operate_FWD_Duty * 10)
            {
              dutyX10 = _Door_Assist_Operate_FWD_Duty * 10;
            }
          }
          cabinP->components.doorAssistOut.speedOrPWM = dutyX10 / 10;
        }
        break;

      case 2:
        if(cabinP->components.doorAssistInputSwitch.value == _SwitchValue_CLOSE)
        {
          cabinP->components.doorAssistOut.speedOrPWM = 10;
          cabinP->values.doorAssist.procTimer = 5; // TIMER 0.1s SET
          cabinP->values.doorAssist.proc = 3;
        }
        else
        {
          if(!cabinP->values.doorAssist.procTimer)
          {
            cabinP->values.doorAssist.proc = 100;
          }
        }
        break;

      case 3:
        if(!cabinP->values.doorAssist.procTimer)
        {
          cabinP->components.doorAssistMotor.value = _DoorAssistMotor_STOP;        //Break Mode
          cabinP->components.doorAssistOut.speedOrPWM = 0;
          cabinP->values.doorAssist.procTimer = _Door_Assist_Wait_OUT_IN_Time;    //Time to wait for IN
          cabinP->values.doorAssist.proc = 99;
        }
        break;


        /*In Process*/

      case 4:
        cabinP->components.doorAssistMotor.value = _DoorAssistMotor_RVS;
        cabinP->components.doorAssistOut.speedOrPWM =
            initMode ? _Door_Assist_Init_Duty : _Door_Assist_Operate_RVS_Duty;

        cabinP->values.doorAssist.procTimer =
            initMode ? _Door_Assist_Init_Timeout : 100; // normal: TIMER 2s SET
        cabinP->values.doorAssist.proc = 5;
        break;

      case 5:
        if(cabinP->components.doorAssistInputSwitch.value == _SwitchValue_OPEN)
        {
          cabinP->values.doorAssist.procTimer = 250; // TIMER 5s SET
          cabinP->values.doorAssist.proc = 6;
        }
        else
        {
          if(!cabinP->values.doorAssist.procTimer)
          {
            cabinP->values.doorAssist.proc = 100;
          }
        }
        break;

      case 6:
        cabinP->components.doorAssistOut.speedOrPWM =
            initMode ? _Door_Assist_Init_Duty : _Door_Assist_last_Duty;
        if(cabinP->components.doorAssistInputSwitch.value == _SwitchValue_CLOSE)
        {
          cabinP->values.doorAssist.procTimer = 5; // TIMER 0.1s SET
          cabinP->values.doorAssist.proc = 7;
        }
        else
        {
          if(!cabinP->values.doorAssist.procTimer)
          {
            cabinP->values.doorAssist.proc = 100;
          }
        }
        break;

      case 7:
        if(!cabinP->values.doorAssist.procTimer)
        {
          cabinP->components.doorAssistMotor.value = _DoorAssistMotor_STOP;        //Break Mode
          cabinP->components.doorAssistOut.speedOrPWM = 0;
          cabinP->values.doorAssist.proc = 8;
        }
        break;


        /*END Process*/
      case 8:
        cabinP->values.doorAssist.procTimer = 0;
        cabinP->values.doorAssist.proc = 0;
        cabinP->components.doorAssistMotor.value = _DoorAssistMotor_STOP;        //Break Mode
        cabinP->components.doorAssistOut.speedOrPWM = 0;
        cabinP->values.doorAssist.delayTimer = _Door_Assist_Delay;
        cabinP->values.doorAssist.request = false;
        if(initMode) { initMode = 0; initDone = 1; }   // init finished
        break;

        /*Wait Process*/
      case 99:
        if(!cabinP->values.doorAssist.procTimer)
        {
          cabinP->values.doorAssist.proc = 4;
        }
        break;


        /*Error Process*/
      case 100:
        cabinP->values.doorAssist.procTimer = 0;
        cabinP->values.doorAssist.proc = 0;
        cabinP->components.doorAssistMotor.value = _DoorAssistMotor_STOP;        //Break Mode
        cabinP->components.doorAssistOut.speedOrPWM = 0;
        cabinP->values.doorAssist.delayTimer = _Door_Assist_Delay;
        cabinP->values.doorAssist.request = false;
        if(initMode) { initMode = 0; initDone = 1; }   // init failed: do not retry (avoid heating)
        break;

      default:
        cabinP->values.doorAssist.proc = 0;
        break;
      }
    }
}
#endif