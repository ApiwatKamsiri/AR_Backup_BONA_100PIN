#include "rf_doors_alert.h"
#include "..\..\mw\connectivity\remote_control.h"

bool doorsOpenedIncreaseLevel(void)
{
  if (refrigerator_R.Cooler.components.solenoidValve[0].positionIndex == 1)
    if(refrigerator_R.Cabin[0].values.doorsOpenedLevelIncreaseTimer >= refrigerator_F.Parameters.doorsAlertLevelIncreaseTime)
      return true;

  if (refrigerator_R.Cooler.components.solenoidValve[0].positionIndex == 2)
    if(refrigerator_R.Cabin[1].values.doorsOpenedLevelIncreaseTimer >= refrigerator_F.Parameters.doorsAlertLevelIncreaseTime)
      return true;
  
  return false;
}

void rfDoorsAlertInit(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    rfP->Values.doorsAlert = false;
    rfP->Values.doorsAlertDisable = false;
#ifdef HOSHI_H30
    if(rfP->Sets.user.functions.EcoExtra)
    {
      rfP->Values.doorsAlertEcoModeFirstTime = true;
      rfP->Values.doorsAlertTimer = 30;         //APW : Fix first alarm to 30s for energy saving (AHTH need to common Gen1)  
    }      
    else
    {
      rfP->Values.doorsAlertEcoModeFirstTime = false;
      rfP->Values.doorsAlertTimer = rfC->Parameters.doorsAlertTime[0];
      rfP->Values.doorAlertBuzzerStart = 1;
    }
    rfP->Values.doorsAlertTimeSelected = 0;     //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
    rfP->Values.doorsAlertCount = 0;            //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
#else   
    rfP->Values.doorsAlertTimer = rfC->Parameters.doorsAlertTime[0];
#endif
}

void rfDoorsAlert1SecTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);
    
    /*if (rfP->Values.doorsAlertTimer)
        rfP->Values.doorsAlertTimer--;*/
    /*** APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1) ***/
#ifdef HOSHI_H30
    if(rfP->Values.doorsAlertTimer)
      rfP->Values.doorsAlertTimer--;
    else
    {
        if(rfP->Values.doorsAlertTimeSelected < 5)
        {
          rfP->Values.doorsAlertTimeSelected++;
          rfP->Values.doorsAlertCount = 0;
          rfP->Values.doorAlertBuzzerStart = 1;
          if(rfP->Sets.user.functions.EcoExtra && rfP->Values.doorsAlertEcoModeFirstTime)
            rfP->Values.doorsAlertTimer = (rfC->Parameters.doorsAlertTime[0] - 30);
          else
            rfP->Values.doorsAlertTimer = (rfC->Parameters.doorsAlertTime[0 + rfP->Values.doorsAlertTimeSelected] - 
                                            rfC->Parameters.doorsAlertTime[rfP->Values.doorsAlertTimeSelected - 1]);
        }
        else
          rfP->Values.doorsAlertTimer = 0;
    }
#else
    if (rfP->Values.doorsAlertTimer)
        rfP->Values.doorsAlertTimer--;
#endif
    /*** APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1) ***/
    
    if (doorsOpenedIncreaseLevel())
    {
      rfP->Values.doorsAlertLevelIncrease = true;
    }
    else
    {
      rfP->Values.doorsAlertLevelIncrease = false;
      rfP->Values.doorsAlertLevelIncreasePrev = false;
    }
}

void rfDoorsAlert1MinTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    if (rfP->Values.doorsAlertingCloseLightsTimer)
        rfP->Values.doorsAlertingCloseLightsTimer--;
}

void rfDoorsAlertTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    if (rfP->Sets.user.functions.Sabbath ||
        rfP->Values.doorsClosed)
    {
        rfP->Values.doorsAlert = false;
        // TODO connectivity
        //rfP->Values.doorsAlertTimer = rfC->Parameters.doorsAlertTime[Wifi.DoorAlarmTime];
#ifdef HOSHI_H30
        if(rfP->Sets.user.functions.EcoExtra)
        {
          rfP->Values.doorsAlertEcoModeFirstTime = true;
          rfP->Values.doorsAlertTimer = 30;         //APW : Fix first alarm to 30s for energy saving (AHTH need to common Gen1)  
        }      
        else
        {
          rfP->Values.doorsAlertEcoModeFirstTime = false;
          rfP->Values.doorsAlertTimer = rfC->Parameters.doorsAlertTime[0];
        }
        rfP->Values.doorsAlertTimeSelected = 0;                 //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        rfP->Values.doorsAlertCount = 0;                        //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
#else
#if (CONNECTIVITY != _NON_CONNECTED)
        rfP->Values.doorsAlertTimer = rfC->Parameters.doorsAlertTime[WiFiVariables.DoorAlarmTime];
#else
        rfP->Values.doorsAlertTimer = rfC->Parameters.doorsAlertTime[0];
#endif
#endif	//HOSHI_H30
        rfP->Values.doorsAlertDisable = false;
        rfP->Values.doorAlertBuzzerStart = 1;
    }

    if (rfP->Values.doorsAlertDisable)
    {
        rfP->Values.doorsAlert = false;
        rfP->Values.doorsAlertTimer = 0;
    }

    if (!rfP->Values.doorsClosed &&
        !rfP->Values.doorsAlertDisable &&
        !rfP->Values.doorsAlertTimer)
    {
        rfP->Values.doorsAlert = true;
    }

    if (!rfP->Values.doorsAlert)
    {
        rfP->Values.doorsAlertingCloseLights = false;
        rfP->Values.doorsAlertingCloseLightsTimer =
            rfC->Parameters.doorsAlertingCloseLightTime;
    }

    /*if (!rfP->Values.doorsAlertingCloseLightsTimer)
        rfP->Values.doorsAlertingCloseLights = true;*/
// For HOSHI_H30 model, we need to disable Alert door close light all mode.
#ifndef HOSHI_H30
    // For BONA model, we need to disable Alert door close light at CT mode.
    if (!rfP->Values.doorsAlertingCloseLightsTimer && 
          rfP->ui.states.commercial.status == _UI_Commercial_None)
        rfP->Values.doorsAlertingCloseLights = true;
#endif
    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        rfP->Cabin[i].values.innerLight.alert =
            rfP->Values.doorsAlert;
        rfP->Cabin[i].values.innerLight.close =
            rfP->Values.doorsAlertingCloseLights;
    }
}
