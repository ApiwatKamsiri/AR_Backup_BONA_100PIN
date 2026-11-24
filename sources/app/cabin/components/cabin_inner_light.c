#include "cabin_inner_light.h"

#define _InnerLight_DutyIncrease 1
#define _InnerLight_DutyDecrease 1

#ifdef HOSHI_H30  
#define _InnerLight_AlertOnTime (0.5 * 50)  // (sec*50) *20msec
#define _InnerLight_AlertOffTime (0.5 * 50) // (sec*50) *20msec
#else
#define _InnerLight_AlertOnTime (0.6 * 50)  // (sec*50) *20msec
#define _InnerLight_AlertOffTime (0.6 * 50) // (sec*50) *20msec
#endif  //APW : Create pattern led alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)

#define _InnerLight_DelayTime (0.3 * 50)    // (sec*50) *20msec
#define _StartDuty              30            //LED Duty Change
#define _FirstDutyTime          (1.5 * 50)      //LED Duty Change (sec*50) *20msec

// unsigned char alert;
unsigned char onTimer;
unsigned char offTimer;

void cabinInnerLightInit(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    onTimer = 0;
    offTimer = 0;
    cabinP->values.innerLight.alert = 0;
    cabinP->values.innerLight.timer = 0;
    cabinP->values.innerLight.state = 0;
    cabinP->values.innerLight.sync = 0;
    // cabinP->components.innerLight.step = INNER_LIGHT_STEP; // 0xFF;
    cabinP->components.innerLight.value = 0;
}

void cabinInnerLight20MsTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    if (cabinP->values.innerLight.timer < 0xFF)
        cabinP->values.innerLight.timer++;
}

void cabinInnerLightTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    // unsigned char step;
    static unsigned char syncState;
    static unsigned char syncTimer;

    if (cabinP->sets.user.functions.Sabbath)
        cabinP->values.innerLight.close = true;

    if (!cabinP->values.innerLight.alert)
        cabinP->values.innerLight.sync = 0;

    if (cabinP->values.doorOpenedTimer > _InnerLight_DelayTime)
    {
        if (cabinP->values.innerLight.close)
        {
            if (cabinP->values.innerLight.state)
            {
                cabinP->values.innerLight.state = 0;
                cabinP->values.innerLight.timer = 0;
            }
        }
        else if (cabinP->values.innerLight.alert)
        {
            if (!cabinP->values.innerLight.sync)
            {
                cabinP->values.innerLight.state = syncState;
                cabinP->values.innerLight.timer = syncTimer;
                cabinP->values.innerLight.sync = 1;
            }

            /*if (cabinP->values.innerLight.state)
            {
                if (cabinP->values.innerLight.timer >= _InnerLight_AlertOnTime)
                {
                    cabinP->values.innerLight.state = 0;
                    cabinP->values.innerLight.timer = 0;
                }
            }
            else
            {
                if (cabinP->values.innerLight.timer >= _InnerLight_AlertOffTime)
                {
                    cabinP->values.innerLight.state = 1;
                    cabinP->values.innerLight.timer = 0;
                }
            }*/
            /*** APW : Create pattern led alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1) ***/
#ifdef HOSHI_H30
            if (cabinP->values.innerLight.state)
            {
                if (cabinP->values.innerLight.timer >= _InnerLight_AlertOnTime)
                {
                    if(refrigerator_R.Values.doorsAlertTimeSelected == 1 &&
                        refrigerator_R.Values.doorsAlertCount < 5)
                    {
                      if(refrigerator_R.Values.doorsAlertCount < 1)
                        cabinP->values.innerLight.state = 1;
                      else
                        cabinP->values.innerLight.state = 0;
                      
                    cabinP->values.innerLight.timer = 0;
                    }                      
                    else if(refrigerator_R.Values.doorsAlertTimeSelected == 2 &&
                        refrigerator_R.Values.doorsAlertCount < 10)
                    {
                      if(refrigerator_R.Values.doorsAlertCount < 1)
                        cabinP->values.innerLight.state = 1;
                      else
                        cabinP->values.innerLight.state = 0;
                      
                    cabinP->values.innerLight.timer = 0;
                    }
                    else if(refrigerator_R.Values.doorsAlertTimeSelected == 3 ||
                              refrigerator_R.Values.doorsAlertTimeSelected == 4 ||
                                refrigerator_R.Values.doorsAlertTimeSelected == 5)
                    {
                      if(refrigerator_R.Values.doorsAlertCount < 1)
                        cabinP->values.innerLight.state = 1;
                      else
                        cabinP->values.innerLight.state = 0;
                      
                    cabinP->values.innerLight.timer = 0;
                    }
                    else
                    {
                      cabinP->values.innerLight.state = 1; 
                      cabinP->values.innerLight.timer = 0xFF;
                    }
                  refrigerator_R.Values.doorsAlertAndDisplay = false;           //Display blink same innerlight
                }
            }
            else
            {
                if (cabinP->values.innerLight.timer >= _InnerLight_AlertOffTime)
                {
                    cabinP->values.innerLight.state = 1;
                    cabinP->values.innerLight.timer = 0;
                  refrigerator_R.Values.doorsAlertAndDisplay = true;           //Display blink same innerlight
                }
            }
#else
            if (cabinP->values.innerLight.state)
            {
                if (cabinP->values.innerLight.timer >= _InnerLight_AlertOnTime)
                {
                    cabinP->values.innerLight.state = 0;
                    cabinP->values.innerLight.timer = 0;
                }
            }
            else
            {
                if (cabinP->values.innerLight.timer >= _InnerLight_AlertOffTime)
                {
                    cabinP->values.innerLight.state = 1;
                    cabinP->values.innerLight.timer = 0;
                }
            }
#endif
            /*** APW : Create pattern led alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1) ***/
        }
        else
        {
            if (!cabinP->values.innerLight.state)
            {
                cabinP->values.innerLight.state = 1;
                cabinP->values.innerLight.timer = 0;
            }
        }

        syncState = cabinP->values.innerLight.state;
        syncTimer = cabinP->values.innerLight.timer;
    }
    else
    {
        if (!cabinP->values.doorClosedDelay)
        {
            cabinP->values.innerLight.state = 0;
            cabinP->components.innerLight.value = 0;
            cabinP->values.innerLight.sync = 0;
            // cabinP->components.innerLight.step = INNER_LIGHT_STEP;
            return;
        }
    }
#ifdef HOSHI_H30
    if (cabinP->values.innerLight.state)
    {
        cabinP->components.innerLight.value = 100; // 100%
        // step = INNER_LIGHT_STEP;   
    }
    else
    {
        cabinP->components.innerLight.value = 0; // 0%
        // step = INNER_LIGHT_STEP; 
    }
#else
    if (cabinP->values.innerLight.state && cabinP->values.doorOpenedTimer )
    {
      if(cabinP->values.doorOpenedTimer < _FirstDutyTime && 
            refrigerator_R.workMode.values.workMode != _WM_ProductionTest)
        cabinP->components.innerLight.value = _StartDuty;                                       // 1st start duty
      else if(cabinP->components.innerLight.value < 100 && 
                refrigerator_R.workMode.values.workMode != _WM_ProductionTest)
        cabinP->components.innerLight.value++;
      else
        cabinP->components.innerLight.value = 100;
        // step = INNER_LIGHT_STEP;   
    }
    else
    {
        cabinP->components.innerLight.value = 0; // 0%
        // step = INNER_LIGHT_STEP; 
    }
#endif
    // cabinP->components.innerLight.step = step;
}
