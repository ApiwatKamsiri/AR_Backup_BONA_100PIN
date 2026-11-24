#include "ui_display.h"
#include "app/libs/temp_funcs.h"
#include "../procedures/ui_errors.h"

// extern int iswifioff;
#define OTAProcessData 0x2B

#define _UI_KeyLockWarningTime (0.5 * 50)
#define _UI_IceOffWarningTime (1 * 50)
#define BLINKTIME 50
#define ANIMATIONTIME 100
extern unsigned char isOTAexistingflag;

void uiDspKeyLock(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;
    stRefrigerator_Ram *rfP = params->rf;
    UNUSED(consts);

    unsigned char LED_Lock,LED_LockDspr = false;
    

    if (!uiP->timers.keyLockWarningTimer)
    {
        if (uiP->timers.keyLockWarningCounter)
            uiP->timers.keyLockWarningCounter--;
        uiP->timers.keyLockWarningTimer = _UI_KeyLockWarningTime;
    }

    if (uiP->timers.keyLockWarningCounter)
    {
        if (uiP->timers.keyLockWarningTimer < (_UI_KeyLockWarningTime / 2))
        {
          if(rfP->Values.keyLock)
            LED_Lock = true;
          if(rfP->Values.keyLockDspr)
            LED_LockDspr = true;
        }
    }
    else
    {
        uiP->timers.keyLockWarningTimer = _UI_KeyLockWarningTime;
        LED_Lock = rfP->Values.keyLock;
        LED_LockDspr = rfP->Values.keyLockDspr;
    }
    uiP->dspPacket.funcs.ledLock = LED_Lock;
    uiP->dspPacket.funcs2.ledLockDspr = LED_LockDspr;
}

void uiDspVacuumBox(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;


    unsigned char LED_VacuumSet_One, LED_VacuumSet_MinusOne = false;
    if(consts->rf->Cabin[0].components.vacuumBox.type)
    {
      if(params->rf->Cabin[0].values.VacuumBox.sets == _VacuumSet_MinusOne)
      {
        LED_VacuumSet_MinusOne = true;
      }
      else if(params->rf->Cabin[0].values.VacuumBox.sets == _VacuumSet_One)
      {
        LED_VacuumSet_One = true;
      }
      else if(params->rf->Cabin[0].values.VacuumBox.sets == _VacuumSet_Off)
      {
        LED_VacuumSet_MinusOne = false;
        LED_VacuumSet_One = false;
      }
    } 
    else if(consts->rf->Cabin[1].components.vacuumBox.type)
    {
      if(params->rf->Cabin[1].values.VacuumBox.sets == _VacuumSet_MinusOne)
      {
        LED_VacuumSet_MinusOne = true;
      }
      else if(params->rf->Cabin[1].values.VacuumBox.sets == _VacuumSet_One)
      {
        LED_VacuumSet_One = true;
      }
      else if(params->rf->Cabin[0].values.VacuumBox.sets == _VacuumSet_Off)
      {
        LED_VacuumSet_MinusOne = false;
        LED_VacuumSet_One = false;
      }
    }

    
    /*** APW ***/
    if(uiP->states.commercial.status != _UI_Commercial_None)
    {
      if(params->rf->IceMachine.values.state == _IMch_CommercialState_TestComplete || 
         params->rf->IceMachine.values.state == _IMch_CommercialState_ReadyToTest)
      {
        if(LED_VacuumSet_MinusOne)
        {
          uiP->dspPacket.funcs.ledCabin2Quick = 0x03;
          uiP->dspPacket.funcs.ledCabin3Quick = 0x00;
        }
        else if(LED_VacuumSet_One)
        {
          uiP->dspPacket.funcs.ledCabin2Quick = 0x00;
          uiP->dspPacket.funcs.ledCabin3Quick = 0x03;
        }
        else
        {
          uiP->dspPacket.funcs.ledCabin2Quick = 0x00;
          uiP->dspPacket.funcs.ledCabin3Quick = 0x00;
        }

      }
    }
    else
    {
        if(LED_VacuumSet_MinusOne)
        {
          uiP->dspPacket.funcs.ledCabin2Quick = 0x03;
          uiP->dspPacket.funcs.ledCabin3Quick = 0x00;
        }
        else if(LED_VacuumSet_One)
        {
          uiP->dspPacket.funcs.ledCabin2Quick = 0x00;
          uiP->dspPacket.funcs.ledCabin3Quick = 0x03;
        }
        else
        {
          uiP->dspPacket.funcs.ledCabin2Quick = 0x00;
          uiP->dspPacket.funcs.ledCabin3Quick = 0x00;
        }
    }
}

void uiDspIceOff(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;
    stRefrigerator_Ram *rfP = params->rf;
    const stRefrigerator_Flash *rfC = consts->rf;

    unsigned char LED_IceOff, LED_QuickIce60 = false;
    
    if (!uiP->timers.iceOffWarningTimer)
    {
        uiP->timers.iceOffWarningTimer = _UI_IceOffWarningTime;
    }

    if (rfC->IceMachine.type)
    {
        if (rfP->IceMachine.values.noWaterSet)
        {
            if(uiP->timers.iceOffWarningTimer < (2 * _UI_IceOffWarningTime / 3))
               LED_IceOff = true;
        }
        else
        {
            LED_IceOff = rfP->IceMachine.sets.iceOFF;
        }
    }

    //uiP->dspPacket.funcs.ledIceOFF = LED_IceOff;
    //uiP->dspPacket.funcs.ledIceOn = !LED_IceOff;
    
    /*** APW ***/
    if(uiP->states.commercial.status != _UI_Commercial_None)
    {
      if(params->rf->IceMachine.values.state == _IMch_CommercialState_TestComplete || 
          params->rf->IceMachine.values.state == _IMch_CommercialState_ReadyToTest)
      {
        uiP->dspPacket.funcs.ledIceOFF = LED_IceOff;
        uiP->dspPacket.funcs.ledIceOn = !LED_IceOff;
      }
    }
    else
    {
      uiP->dspPacket.funcs.ledIceOFF = LED_IceOff;
      uiP->dspPacket.funcs.ledIceOn = !LED_IceOff;
    }
    /*** APW ***/
    
    LED_QuickIce60 = rfP->IceMachine.values.QuickIce60;
    uiP->dspPacket.funcs.ledFastBottleCooling = LED_QuickIce60;
}

void uiDspIceCabinsSetsOff(const tsUiProcConsts *consts, tsUiProcParams *params)
{
#ifdef HOSHI_H30  
    if ((params->rf->workMode.values.workMode == _WM_Distributer) && (!params->ui->timers.dspOffTimer))       ////Demo Display
      return;
#endif //HOSHI_H30
    for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
        uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
}

void uiDspIceCabinsSetsOn(const tsUiProcConsts *consts, tsUiProcParams *params)
{
#ifdef HOSHI_H30  
    if ((params->rf->workMode.values.workMode == _WM_Distributer) && (!params->ui->timers.dspOffTimer))       //Demo Display
      return;  
#endif //HOSHI_H30  
    for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
        uiDspCabinData(consts, params, eDisplaySpecialsAllOn, i, eDisplayCabinDataTypeSpecial);
}

void uiDspFunctionsAtOffMode(const tsUiProcConsts *consts, tsUiProcParams *params)
{
#ifdef HOSHI_H30
    if ((params->rf->workMode.values.workMode == _WM_Distributer) && (!params->ui->timers.dspOffTimer))       //Demo Display
      return;
#endif //HOSHI_H30      
    
    tsUiRam *uiP = params->ui;
    stRefrigerator_Ram *rfP = params->rf;

    uiP->dspPacket.funcs.ledEconomy = false;
    uiP->dspPacket.funcs.ledEcoExtra = false;
    uiP->dspPacket.funcs.ledVacation = false;
    uiP->dspPacket.funcs.ledIceOFF = false;
    uiP->dspPacket.funcs.ledIceOn = false;                      //APW : Add Ice making operation test in CT
    uiP->dspPacket.funcs.ledFastBottleCooling = false;          //APW : Add Ice making operation test in CT
    //uiP->dspPacket.funcs.ledAlert = false;    //APW : Disable for BONA
    uiP->dspPacket.funcs.ledDispOff = rfP->Values.dispOff;
    uiP->dspPacket.funcs.ledFilter = false;
    uiP->dspPacket.funcs.ledConverter = false;
    uiP->dspPacket.funcs.ledServiceButton = false;

    uiP->dspPacket.funcs.ledCabin0Quick = 0;
    uiP->dspPacket.funcs.ledCabin1Quick = 0;
    uiP->dspPacket.funcs.ledCabin2Quick = 0;
    uiP->dspPacket.funcs.ledCabin3Quick = 0;
    // uiP->dspPacket.funcs.ledCabin4Quick  = 0;

    uiP->dspPacket.funcs.ledIceCream = 0;
    uiP->dspPacket.funcs.ledCabin0Icon = 0;
    uiP->dspPacket.funcs.ledCabin1Icon = 0;
    uiP->dspPacket.funcs.ledCabin2Icon = 0;
    uiP->dspPacket.funcs.ledCabin3Icon = 0;

    uiP->dspPacket.funcs.ledWifi = false;
    // uiP->dspPacket.funcs.ledSilent = false;
      uiDspDispenser(consts, params);
    /*
    uiP->dspPacket.funcs.ledWater = false;
    uiP->dspPacket.funcs.ledCubicIce = false;
    uiP->dspPacket.funcs.ledSplitIce = false;
    uiP->dspPacket.funcs.ledDispenserSW = true;
    */
}

void uiDspFunctionsOff(const tsUiProcConsts *consts, tsUiProcParams *params)
{
#ifdef HOSHI_H30
    if ((params->rf->workMode.values.workMode == _WM_Distributer) && (!params->ui->timers.dspOffTimer))       //Demo Display
      return;
#endif //HOSHI_H30   
    
    tsUiRam *uiP = params->ui;
    UNUSED(consts);

    uiP->dspPacket.funcs.ledEconomy = false;
    uiP->dspPacket.funcs.ledEcoExtra = false;
    uiP->dspPacket.funcs.ledVacation = false;
    uiP->dspPacket.funcs.ledIceOFF = false;
    uiP->dspPacket.funcs.ledIceOn = false;                      //APW : Add Ice making operation test in CT
    uiP->dspPacket.funcs.ledFastBottleCooling = false;          //APW : Add Ice making operation test in CT
    //uiP->dspPacket.funcs.ledAlert = false;                    //APW : Disable for BONA     
    uiP->dspPacket.funcs.ledLock = false;
    uiP->dspPacket.funcs2.ledLockDspr = false;
    uiP->dspPacket.funcs.ledDispOff = false;
    uiP->dspPacket.funcs.ledFilter = false;
    uiP->dspPacket.funcs.ledConverter = false;
    uiP->dspPacket.funcs.ledSplitIce = false;
    uiP->dspPacket.funcs.ledCubicIce = false;
    uiP->dspPacket.funcs.ledWater = false;
    uiP->dspPacket.funcs.ledRice = false;
    uiP->dspPacket.funcs.ledServiceButton = false;
    uiP->dspPacket.funcs.ledCelsius = false;
    uiP->dspPacket.funcs.ledFahrenheit = false;

    uiP->dspPacket.funcs.ledCabin0Quick = 0;
    uiP->dspPacket.funcs.ledCabin1Quick = 0;
    uiP->dspPacket.funcs.ledCabin2Quick = 0;
    uiP->dspPacket.funcs.ledCabin3Quick = 0;
    // uiP->dspPacket.funcs.ledCabin4Quick  = 0;

    uiP->dspPacket.funcs.ledIceCream = 0;
    uiP->dspPacket.funcs.ledCabin0Icon = 0;
    uiP->dspPacket.funcs.ledCabin1Icon = 0;
    uiP->dspPacket.funcs.ledCabin2Icon = 0;
    uiP->dspPacket.funcs.ledCabin3Icon = 0;
    uiP->dspPacket.funcs.ledWifi = false;
    uiP->dspPacket.funcs.ledDispenserSW = false;
    
    uiP->dspPacket.funcs2.led8 = false;
    uiP->dspPacket.funcs2.led16 = false;
    uiP->dspPacket.funcs2.led32 = false;
    uiP->dspPacket.funcs2.ledCustom = false;
}

void uiDspFunctionsOn(const tsUiProcConsts *consts, tsUiProcParams *params)
{
#ifdef HOSHI_H30
    if ((params->rf->workMode.values.workMode == _WM_Distributer) && (!params->ui->timers.dspOffTimer))       //Demo Display
      return;
#endif //HOSHI_H30  
    
    tsUiRam *uiP = params->ui;
    UNUSED(consts);

    uiP->dspPacket.funcs.ledEconomy = true;
    uiP->dspPacket.funcs.ledEcoExtra = true;
    uiP->dspPacket.funcs.ledVacation = true;
    uiP->dspPacket.funcs.ledIceOFF = true;
    uiP->dspPacket.funcs.ledIceOn = true;               //APW : Add Ice making operation test in CT
    uiP->dspPacket.funcs.ledFastBottleCooling = true;   //APW : Add Ice making operation test in CT
    //uiP->dspPacket.funcs.ledAlert = true;             //APW : Disable for BONA  
    uiP->dspPacket.funcs.ledLock = true;
    uiP->dspPacket.funcs2.ledLockDspr = true;
    uiP->dspPacket.funcs.ledDispOff = false;
    uiP->dspPacket.funcs.ledFilter = true;
    uiP->dspPacket.funcs.ledConverter = true;
    uiP->dspPacket.funcs.ledSplitIce = true;
    uiP->dspPacket.funcs.ledCubicIce = true;
    uiP->dspPacket.funcs.ledWater = true;
    uiP->dspPacket.funcs.ledRice = true;
    uiP->dspPacket.funcs.ledServiceButton = true;
    uiP->dspPacket.funcs.ledCelsius = true;
    uiP->dspPacket.funcs.ledFahrenheit = true;

    uiP->dspPacket.funcs.ledCabin0Quick = 0x03;
    uiP->dspPacket.funcs.ledCabin1Quick = 0x03;
    uiP->dspPacket.funcs.ledCabin2Quick = 0x03;
    uiP->dspPacket.funcs.ledCabin3Quick = 0x03;
    // uiP->dspPacket.funcs.ledCabin4Quick  = 0x03;

    uiP->dspPacket.funcs.ledIceCream = 1;
    uiP->dspPacket.funcs.ledCabin0Icon = 1; // 0x1F;
    uiP->dspPacket.funcs.ledCabin1Icon = 1; // 0x1F;
    uiP->dspPacket.funcs.ledCabin2Icon = 1; // 0x1F;
    uiP->dspPacket.funcs.ledCabin3Icon = 1; // 0x1F;
    uiP->dspPacket.funcs.ledWifi = true;
    uiP->dspPacket.funcs.ledDispenserSW = true;
    
    uiP->dspPacket.funcs2.led8 = true;
    uiP->dspPacket.funcs2.led16 = true;
    uiP->dspPacket.funcs2.led32 = true;
    uiP->dspPacket.funcs2.ledCustom = true;
}

void uiDspAllOff(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    uiDspIceCabinsSetsOff(consts, params);
    uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);
    uiDspFunctionsOff(consts, params);
}

void uiDspAllOn(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    uiDspIceCabinsSetsOn(consts, params);
    uiDspIceCreamData(consts, params, eDisplaySpecialsAllOn, eDisplayCabinDataTypeSpecial);
    uiDspFunctionsOn(consts, params);
}

unsigned char uiDspCheckTemp(const tsUiProcConsts *consts, tsUiProcParams *params, signed char NTC_Value)
{
    UNUSED(consts);
    UNUSED(params);
    return NTC_Value;
}

void uiDspCabinsQuicks(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    stRefrigerator_Ram *rfP = params->rf;
    const stRefrigerator_Flash *rfC = consts->rf;

    for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
    {
        if (rfC->Cabin[i].type == _CabinType_NONE)
            uiDspCabinQuickLED(consts, params, i, eDisplayLedStateOff);
        else
        {
            if (rfP->Cabin[i].sets.user.functions.Quick)
                uiDspCabinQuickLED(consts, params, i, eDisplayLedStateAnimation);
            else
                uiDspCabinQuickLED(consts, params, i, eDisplayLedStateOff);
        }
    }
}

void uiDspCabinsSets(const tsUiProcConsts *consts, tsUiProcParams *params)
{
#ifdef HOSHI_H30
    if ((params->rf->workMode.values.workMode == _WM_Distributer) && (!params->ui->timers.dspOffTimer))       //Demo Display
      return;
#endif //HOSHI_H30 
    
    unsigned char /*dummyPointerFF0, dummyPointerFRZ0,*/ dummyPointerFF1, dummyPointerFRZ1;
    stRefrigerator_Ram *rfP = params->rf;
    const stRefrigerator_Flash *rfC = consts->rf;
    /*
    if (display_F.SubType == _DisplayType_DUMMY_v0)
    {
        for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
        {
            if (i == rfC->Parameters.displayData)
            {
                if (rfC->Cabin[uiP->dspValues.dspSelector].type == _CabinType_NONE)
                {
                    uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
                }
                else
                {
                    if (rfP->Cabin[uiP->dspValues.dspSelector].sets.user.functions.Vacation)
                    {
                        uiDspCabinData(consts, params, eDisplaySpecialsMidLine, i, eDisplayCabinDataTypeSpecial);
                    }
                    else if (rfP->Cabin[uiP->dspValues.dspSelector].sets.user.functions.Quick)
                    {
                        switch (rfP->Cabin[uiP->dspValues.dspSelector].sets.user.Mode)
                        {
                        case _CabinMode_FF:
                            dummyPointerFF0 = rfP->Values.temperatureConvPointer[i];
                            setpointerValue(rfP, i, fahrenheitCelciusTable.QFRD_ValueCelcius);
                            uiDspCabinData(consts, params, (unsigned char)uiDspTemperature(consts, params, 1, i), i, eDisplayCabinDataTypeNumber);
                            rfP->Values.temperatureConvPointer[i] = dummyPointerFF0;
                            break;
                        case _CabinMode_FRZ:
                            dummyPointerFRZ0 = rfP->Values.temperatureConvPointer[i];
                            setpointerValue(rfP, i, fahrenheitCelciusTable.QFRZ_ValueCelcius);
                            uiDspCabinData(consts, params, (unsigned char)uiDspTemperature(consts, params, -27, i), i, eDisplayCabinDataTypeNumber);
                            rfP->Values.temperatureConvPointer[i] = dummyPointerFRZ0;
                            break;
                        default:
                            uiDspCabinData(consts, params, eDisplaySpecialsMidLine, i, eDisplayCabinDataTypeSpecial);
                            break;
                        }
                    }
                    else
                    {
                        uiDspCabinData(consts, params, (unsigned char)uiDspTemperature(consts, params, rfP->Cabin[uiP->dspValues.dspSelector].sets.datas.display, uiP->dspValues.dspSelector),
                                            i,
                                            eDisplayCabinDataTypeNumber);
                    }
                }
            }
            else
            {
                uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
            }
        }
    }
    else
    */
    {
        for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
        {
            if (rfC->Cabin[i].type == _CabinType_NONE)
            {
                uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
            }
            else
            {
                if (rfP->Cabin[i].sets.user.functions.Vacation)
                {
                    uiDspCabinData(consts, params, eDisplaySpecialsMidLine, i, eDisplayCabinDataTypeSpecial);
                }
                else if (rfP->Cabin[i].sets.user.functions.Quick)
                {
                    switch (rfP->Cabin[i].sets.user.Mode)
                    {
                    case _CabinMode_FF:
                        dummyPointerFF1 = rfP->Values.temperatureConvPointer[i];
                        setpointerValue(rfP, i, fahrenheitCelciusTable.QFRD_ValueCelcius);
                        // if (!rfP->Cabin[1].sets.user.functions.Convertible) //bug çıkıyor FRZ convertible ise disp off da displayde veri gösterilmiyor
                        uiDspCabinData(consts, params, (unsigned char)uiDspTemperature(consts, params, fahrenheitCelciusTable.QFRD_ValueCelcius/*1*/, i), i, eDisplayCabinDataTypeNumber);
                        rfP->Values.temperatureConvPointer[i] = dummyPointerFF1;
                        break;
                    case _CabinMode_FRZ:
                        dummyPointerFRZ1 = rfP->Values.temperatureConvPointer[i];
                        setpointerValue(rfP, i, fahrenheitCelciusTable.QFRZ_ValueCelcius);
                        uiDspCabinData(consts, params, (unsigned char)uiDspTemperature(consts, params, fahrenheitCelciusTable.QFRZ_ValueCelcius/*-27*/, i), i, eDisplayCabinDataTypeNumber);
                        rfP->Values.temperatureConvPointer[i] = dummyPointerFRZ1;
                        break;
                    default:
                        uiDspCabinData(consts, params, eDisplaySpecialsMidLine, i, eDisplayCabinDataTypeSpecial);
                        break;
                    }
                }
                else if (rfP->Cabin[1].sets.user.functions.CabinOff)
                {
                    uiDspCabinData(consts, params, eDisplaySpecialsMidLine, 1, eDisplayCabinDataTypeSpecial);
                    if (rfP->Cabin[0].sets.user.functions.Vacation)
                        uiDspCabinData(consts, params, eDisplaySpecialsMidLine, 0, eDisplayCabinDataTypeSpecial);
                    else
                        uiDspCabinData(consts, params, (unsigned char)uiDspTemperature(consts, params, rfP->Cabin[0].sets.datas.display, 0), 0, eDisplayCabinDataTypeNumber);
                }
                else
                {
                    uiDspCabinData(consts, params, (unsigned char)uiDspTemperature(consts, params, rfP->Cabin[i].sets.datas.display, i),
                                   i,
                                   eDisplayCabinDataTypeNumber);
                }
            }
        }
    }
    if(isOTAexistingflag)
      uiDspCabinData(consts, params, eDisplaySpecialsAllOTA, 0, eDisplayCabinDataTypeSpecial);
}

/*** APW : Add display setting in CT ***/
void uiDspCabinsSetsInCommercial(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    stRefrigerator_Ram *rfP = params->rf;
    const stRefrigerator_Flash *rfC = consts->rf;

        for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
        {
            if (rfC->Cabin[i].type == _CabinType_NONE)
            {
                uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
            }
            else
            {
                if (rfP->Cabin[i].sets.user.functions.Vacation)
                {
                    uiDspCabinData(consts, params, eDisplaySpecialsMidLine, i, eDisplayCabinDataTypeSpecial);
                }
                else if (rfP->Cabin[i].sets.user.functions.Quick)
                {
                    switch (rfP->Cabin[i].sets.user.Mode)
                    {
                    case _CabinMode_FF:
                        uiDspCabinData(consts, params, (unsigned char)uiDspTemperature(consts, params, fahrenheitCelciusTable.QFRD_ValueCelcius/*1*/, i), i, eDisplayCabinDataTypeNumber);
                        break;
                    case _CabinMode_FRZ:
                        uiDspCabinData(consts, params, (unsigned char)uiDspTemperature(consts, params, fahrenheitCelciusTable.QFRZ_ValueCelcius/*-27*/, i), i, eDisplayCabinDataTypeNumber);
                        break;
                    default:
                        uiDspCabinData(consts, params, eDisplaySpecialsMidLine, i, eDisplayCabinDataTypeSpecial);
                        break;
                    }
                }
                else if (rfP->Cabin[1].sets.user.functions.CabinOff)
                {
                    uiDspCabinData(consts, params, eDisplaySpecialsMidLine, 1, eDisplayCabinDataTypeSpecial);
                    if (rfP->Cabin[0].sets.user.functions.Vacation)
                        uiDspCabinData(consts, params, eDisplaySpecialsMidLine, 0, eDisplayCabinDataTypeSpecial);
                    else
                        uiDspCabinData(consts, params, (unsigned char)uiDspTemperature(consts, params, rfP->Cabin[0].sets.datas.display, 0), 0, eDisplayCabinDataTypeNumber);
                }
                else
                {
                    uiDspCabinData(consts, params, (unsigned char)uiDspTemperature(consts, params, rfP->Cabin[i].sets.datas.display, i),
                                   i,
                                   eDisplayCabinDataTypeNumber);
                }
            }
        }
}
/*** APW : Add display setting in CT ***/

void uiDspCabinsRealTemps(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    const stRefrigerator_Flash *rfC = consts->rf;
    stRefrigerator_Ram *rfP = params->rf;

    for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
    {
        if (rfC->Cabin[i].type == _CabinType_NONE)
        {
            uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
        }
        else
        {
            uiDspCabinData(consts, params,
                           (unsigned char)uiDspTemperature(consts, params,
                                                           celciusExtToCelcius(rfP->Cabin[i].components.airTempSensor.valueInCExt),
                                                           i),
                           i,
                           eDisplayCabinDataTypeNumber);
        }
    }
}

void uiDspError(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;
    const stRefrigerator_Flash *rfC = consts->rf;

    uiResetDisplayOffTimer(uiP);
    uiErrorsControlSelected(consts, params);

    uiP->errors.errorShowState++;
    for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
    {
        if (i == rfC->Parameters.displayData)
        {
            uiDspCabinData(consts, params, uiP->slides.slide[0].data, i, uiP->slides.slide[0].dataType);
        }
        else if (i == rfC->Parameters.displayValue)
        {
            uiDspCabinData(consts, params, uiP->slides.slide[1].data, i, uiP->slides.slide[1].dataType);
        }
        else
        {
            uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
        }
    }

    if (uiP->errors.errorShowState > 24)
        uiP->errors.errorShowState = 0;
}

void uiDspCabinQuickLED(const tsUiProcConsts *consts, tsUiProcParams *params, unsigned char cabinIndex, teDisplayLedState ledx)
{
#ifdef HOSHI_H30
    if ((params->rf->workMode.values.workMode == _WM_Distributer) && (!params->ui->timers.dspOffTimer))       //Demo Display
      return;
#endif //HOSHI_H30  
    
    tsUiRam *uiP = params->ui;

    if ((cabinIndex < DSP_USED_CABINS_COUNT) && (ledx < eDisplayLedStateCount))
    {
        switch (cabinIndex)
        {
        case 0:
            uiP->dspPacket.funcs.ledCabin0Quick = 0x00;
            uiP->dspPacket.funcs.ledCabin0Quick |= ledx;
            break;

        case 1:
            uiP->dspPacket.funcs.ledCabin1Quick = 0x00;
            uiP->dspPacket.funcs.ledCabin1Quick |= ledx;
            break;

       /* case 2:         // Cabin2Quick and Cabin3Quick is used as VacuumBox -1 1 
            uiP->dspPacket.funcs.ledCabin2Quick = 0x00;
            uiP->dspPacket.funcs.ledCabin2Quick |= ledx;
            break;

        case 3:
            uiP->dspPacket.funcs.ledCabin3Quick = 0x00;
            uiP->dspPacket.funcs.ledCabin3Quick |= ledx;
            break;*/

            /*case 4:
                uiP->dspPacket.funcs.ledCabin4Quick = 0x00;
                uiP->dspPacket.funcs.ledCabin4Quick |= ledx;
            break;*/
        }
    }
}

void uiDspIceCreamLED(const tsUiProcConsts *consts, tsUiProcParams *params, teDisplayLedState ledx)
{
    tsUiRam *uiP = params->ui;
    UNUSED(consts);
    if (ledx < eDisplayLedStateCount)
    {
        uiP->dspPacket.funcs.ledIceCream = ledx;
    }
}

void uiDspCabinData(const tsUiProcConsts *consts, tsUiProcParams *params, unsigned char data,
                    unsigned char cabinIndex,
                    teDisplayCabinDataTypes dataType)
{
    tsUiRam *uiP = params->ui;

    if ((cabinIndex < DSP_USED_CABINS_COUNT) && (dataType < eDisplayCabinDataTypesCount))
    {
        // TODO connectivity
         if (!isOTAexistingflag)
        {
          if (params->rf->Sets.user.functions.Sabbath)
            uiP->dspPacket.cabinData[cabinIndex] = 0;
          else
            uiP->dspPacket.cabinData[cabinIndex] = data;
            uiP->dspPacket.cabinDataType =
                (uiP->dspPacket.cabinDataType & ~(1 << cabinIndex)) | (dataType << cabinIndex);
        }
        // TODO connectivity
         else
         {
             uiP->dspPacket.cabinData[cabinIndex] = OTAProcessData;
             uiP->dspPacket.cabinDataType =
                 (uiP->dspPacket.cabinDataType & ~(1 << cabinIndex)) | (eDisplayCabinDataTypeSpecial << cabinIndex);
             isOTAexistingflag = 2; // This flag represent that flashing can continue
         }
    }
}

void uiDspIceCreamData(const tsUiProcConsts *consts, tsUiProcParams *params, unsigned char data,
                       teDisplayCabinDataTypes dataType)
{
    tsUiRam *uiP = params->ui;
    UNUSED(consts);

    if (dataType < eDisplayCabinDataTypesCount)
    {
        uiP->dspPacket.cabinData[eDisplayCabinIceCream] = data;
        uiP->dspPacket.cabinDataType &= ~(1 << eDisplayCabinIceCream);       // clear ice cream bit 5th bit
        uiP->dspPacket.cabinDataType |= (dataType << eDisplayCabinIceCream); // set ice cream bit according to data type value
    }
}

signed char uiDspTemperature(const tsUiProcConsts *consts, tsUiProcParams *params, signed char ValuInC, unsigned char cabinIndex)
{
    stRefrigerator_Ram *rfP = params->rf;
    UNUSED(consts);

    switch (rfP->Values.tempType)
    {
    case _TempType_Fahrenheit:
        return getFahrenheitValue(rfP->Values.temperatureConvPointer[cabinIndex]);

    case _TempType_Celcius:
    default:
        break;
    }

    return ValuInC;
}

void uiDspTempUnit(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    stRefrigerator_Ram *rfP = params->rf;

    unsigned char label;
    switch (rfP->Values.tempType)
    {
    case _TempType_Celcius:
        label = eDisplaySpecialsCDegree;
        break;
    case _TempType_Fahrenheit:
        label = eDisplaySpecialsFDegree;
        break;
    }

    for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
        uiDspCabinData(consts, params, label, i, eDisplayCabinDataTypeSpecial);
}
void uiDspDispenser(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;
    //stRefrigerator_Ram *rfP = params->rf;
    UNUSED(consts);
    
  if(uiP->dspPacket.funcs.displayOff)
  {
    uiP->dspPacket.funcs2.led8 = false;
    uiP->dspPacket.funcs2.led16= false;
    uiP->dspPacket.funcs2.led32 = false;
    uiP->dspPacket.funcs2.ledCustom = false;
    uiP->dspPacket.funcs.ledWaterDispOff = ( params->rf->Dispenser.sets.state == _DispenserState_WaterPump ) ? true : false;
    uiP->dspPacket.funcs.ledCubicIceDispOff = ( params->rf->Dispenser.sets.state == _DispenserState_IceCube ) ? true : false;
    uiP->dspPacket.funcs.ledSplitIceDispOff = ( params->rf->Dispenser.sets.state == _DispenserState_IceCrash ) ? true : false;
    uiP->dspPacket.funcs.ledWater = false;
    uiP->dspPacket.funcs.ledCubicIce = false;
    uiP->dspPacket.funcs.ledSplitIce = false;
  }
  else
  {
    uiP->dspPacket.funcs2.led8 = ( params->rf->Dispenser.sets.state == _DispenserState_Measured_8OZ ||
                                    params->rf->Dispenser.sets.state == _DispenserState_Measured_16OZ ||
                                    params->rf->Dispenser.sets.state == _DispenserState_Measured_32OZ ) ? true : false;
    uiP->dspPacket.funcs2.led16 = ( params->rf->Dispenser.sets.state == _DispenserState_Measured_16OZ ||
                                     params->rf->Dispenser.sets.state == _DispenserState_Measured_32OZ ) ? true : false;
    uiP->dspPacket.funcs2.led32 = ( params->rf->Dispenser.sets.state == _DispenserState_Measured_32OZ ) ? true : false;
    /*uiP->dspPacket.funcs2.ledCustom = ( params->rf->Dispenser.sets.state == _DispenserState_Measured_Custom ||
                                         params->rf->Dispenser.sets.state == _DispenserState_Measured_CalibrateCustom) ? true : false;*/
    uiP->dspPacket.funcs.ledWater = (params->rf->Dispenser.sets.state == _DispenserState_WaterPump) ? true : false;
    uiP->dspPacket.funcs.ledCubicIce = (params->rf->Dispenser.sets.state == _DispenserState_IceCube) ? true : false;
    uiP->dspPacket.funcs.ledSplitIce = (params->rf->Dispenser.sets.state == _DispenserState_IceCrash) ? true : false;
    uiP->dspPacket.funcs.ledWaterDispOff = false;
    uiP->dspPacket.funcs.ledCubicIceDispOff = false;
    uiP->dspPacket.funcs.ledSplitIceDispOff = false;
    uiP->dspPacket.funcs2.ledCustom = false;  
  }
  if(refrigerator_R.Dispenser.components.light.value)
    uiP->dspPacket.funcs.ledDispenserSW = true;
  else
    uiP->dspPacket.funcs.ledDispenserSW = false;
  
  if(params->rf->Dispenser.sets.state == _DispenserState_Measured_8OZ)
  {
    if(!(/*params->rf->Dispenser.components.tactSwitch.value ||*/ params->rf->Dispenser.values.dispenserTactFlag))
    {
      uiP->dspPacket.funcs2.led8 = true;
      uiP->dspPacket.funcs2.ledCustom = true; 
    }
    else
    {
      if(uiP->timers.customCalibrateTimer > 75)
      {       
        uiP->dspPacket.funcs2.led8 = false;
      }
      else if(uiP->timers.customCalibrateTimer > 50)
      {
        uiP->dspPacket.funcs2.led8 = true;
      }
      else
        uiP->timers.customCalibrateTimer = ANIMATIONTIME;
      
      uiP->dspPacket.funcs2.ledCustom = true;
    }
  }
  else if(params->rf->Dispenser.sets.state == _DispenserState_Measured_16OZ)
  {
    if(!(/*params->rf->Dispenser.components.tactSwitch.value ||*/ params->rf->Dispenser.values.dispenserTactFlag))
    {
      uiP->dspPacket.funcs2.led8 = true;
      uiP->dspPacket.funcs2.led16 = true;
      uiP->dspPacket.funcs2.ledCustom = true; 
    }
    else
    {
      if(uiP->timers.customCalibrateTimer > 75)
      {       
        uiP->dspPacket.funcs2.led8 = false;
        uiP->dspPacket.funcs2.led16 = false;
      }
      else if(uiP->timers.customCalibrateTimer > 50)
      {
        uiP->dspPacket.funcs2.led8 = true;
        uiP->dspPacket.funcs2.led16 = false;
      }
      else if(uiP->timers.customCalibrateTimer > 25)
      {
        uiP->dspPacket.funcs2.led8 = true;
        uiP->dspPacket.funcs2.led16 = true;
      }
      else
        uiP->timers.customCalibrateTimer = ANIMATIONTIME;
      
      uiP->dspPacket.funcs2.ledCustom = true; 
    }
    
  }
  else if(params->rf->Dispenser.sets.state == _DispenserState_Measured_32OZ)
  {
    if(!(/*params->rf->Dispenser.components.tactSwitch.value ||*/ params->rf->Dispenser.values.dispenserTactFlag))
    {
      uiP->dspPacket.funcs2.led8 = true;
      uiP->dspPacket.funcs2.led16 = true;
      uiP->dspPacket.funcs2.led32 = true;
      uiP->dspPacket.funcs2.ledCustom = true; 
    }
    else
    {
      if(uiP->timers.customCalibrateTimer > 75)
      {
        uiP->dspPacket.funcs2.led8 = false;
        uiP->dspPacket.funcs2.led16 = false;
        uiP->dspPacket.funcs2.led32 = false;
      }
      else if(uiP->timers.customCalibrateTimer > 50)
      {
        uiP->dspPacket.funcs2.led8 = true;
        uiP->dspPacket.funcs2.led16 = false;
        uiP->dspPacket.funcs2.led32 = false;
      }
      else if(uiP->timers.customCalibrateTimer > 25)
      {
        uiP->dspPacket.funcs2.led8 = true;
        uiP->dspPacket.funcs2.led16 = true;
        uiP->dspPacket.funcs2.led32 = false;
      }
      else
      {
        uiP->dspPacket.funcs2.led8 = true;
        uiP->dspPacket.funcs2.led16 = true;
        uiP->dspPacket.funcs2.led32 = true;
      }
              
      if(!uiP->timers.customCalibrateTimer)
        uiP->timers.customCalibrateTimer = ANIMATIONTIME;
      
      uiP->dspPacket.funcs2.ledCustom = true;
    }
  }
  
    if(params->rf->Values.keyLockDspr)
    {
      uiP->dspPacket.funcs2.led8 = false;
      uiP->dspPacket.funcs2.led16 = false;
      uiP->dspPacket.funcs2.led32 = false;
      uiP->dspPacket.funcs.ledWater = false;
      uiP->dspPacket.funcs.ledCubicIce = false;
      uiP->dspPacket.funcs.ledSplitIce = false;
      uiP->dspPacket.funcs.ledDispenserSW = false;
      uiP->dspPacket.funcs2.ledCustom = false;  
      
    }
  
    if((uiP->states.normal.state == _UI_NormalState_DisplayOff && uiP->states.commercial.status == _UI_Commercial_None) || 
          (uiP->states.commercial.state == _UI_CommercialState_DisplayOff && uiP->states.commercial.status != _UI_Commercial_None))
    {
      uiP->dspPacket.funcs2.led8 = false;
      uiP->dspPacket.funcs2.led16= false;
      uiP->dspPacket.funcs2.led32 = false;
      uiP->dspPacket.funcs2.ledCustom = false;
      uiP->dspPacket.funcs.ledWaterDispOff = ( params->rf->Dispenser.sets.state == _DispenserState_WaterPump ) ? true : false;
      uiP->dspPacket.funcs.ledCubicIceDispOff = ( params->rf->Dispenser.sets.state == _DispenserState_IceCube ) ? true : false;
      uiP->dspPacket.funcs.ledSplitIceDispOff = ( params->rf->Dispenser.sets.state == _DispenserState_IceCrash ) ? true : false;
      uiP->dspPacket.funcs.ledWater = false;
      uiP->dspPacket.funcs.ledCubicIce = false;
      uiP->dspPacket.funcs.ledSplitIce = false;
      uiP->dspPacket.funcs.ledDispenserSW = false;
    }
    else
    { 
      //APW : In CT DEF mode not need
      if(uiP->states.commercial.status != _UI_Commercial_DefrostTest)
        uiP->dspPacket.funcs.ledDispenserSW = true;
    }
    
  
    if(refrigerator_R.Dispenser.values.dispenserProhibitFlag)
    {
      if(refrigerator_R.Dispenser.values.dispenserProhibitBlinkFlag) 
        uiP->dspPacket.funcs.ledDispenserSW = false;
      else
        uiP->dspPacket.funcs.ledDispenserSW = true;
    }
  
//  if(params->rf->Dispenser.sets.state == _DispenserState_Measured_CalibrateCustom)
//  {
//    if(!refrigerator_R.Dispenser.components.tactSwitch.value)
//    {
//      if(uiP->timers.customCalibrateTimer > BLINKTIME/2)
//      {
//        uiP->dspPacket.funcs2.ledCustom = true;    
//      }
//      else
//        uiP->dspPacket.funcs2.ledCustom = false;
//      
//      if(!uiP->timers.customCalibrateTimer)
//        uiP->timers.customCalibrateTimer = BLINKTIME;
//    }
//    else //ANIMATIONTIME
//    {
//      if(uiP->timers.customCalibrateTimer > 75)
//      {       
//        uiP->dspPacket.funcs2.led8 = true;
//        uiP->dspPacket.funcs2.led16 = true;
//      }
//      else if(uiP->timers.customCalibrateTimer > 50)
//      {
//        uiP->dspPacket.funcs2.led8 = true;
//        uiP->dspPacket.funcs2.led16 = true;
//        uiP->dspPacket.funcs2.led32 = true;
//      }
//      else if(uiP->timers.customCalibrateTimer > 25)
//      {
//        uiP->dspPacket.funcs2.led8 = true;
//        uiP->dspPacket.funcs2.led16 = true;
//        uiP->dspPacket.funcs2.led32 = true;
//        uiP->dspPacket.funcs2.ledCustom = true;
//      }
//      else /*if(uiP->timers.customCalibrateTimer > 25)*/
//      {       
//        uiP->dspPacket.funcs2.ledCustom = false;  
//        uiP->dspPacket.funcs2.led8 = true;
//      }
//      /*else
//      {
//      uiP->dspPacket.funcs2.ledCustom = false; 
//      uiP->dspPacket.funcs2.led32 = false;
//      uiP->dspPacket.funcs2.led16 = false;
//      uiP->dspPacket.funcs2.led8 = false;
//    }*/
//      
//      if(!uiP->timers.customCalibrateTimer)
//        uiP->timers.customCalibrateTimer = ANIMATIONTIME;       
//    }   
//  } 
//  else if(params->rf->Dispenser.sets.state == _DispenserState_Measured_Custom)
//  {
//    if(!(params->rf->Dispenser.components.tactSwitch.value || params->rf->Dispenser.values.dispenserTactFlag))
//      uiP->dspPacket.funcs2.ledCustom = true;
//    else
//    {
//      if(uiP->timers.customCalibrateTimer > 75)
//      {       
//        uiP->dspPacket.funcs2.led8 = true;
//        uiP->dspPacket.funcs2.led16 = true;
//      }
//      else if(uiP->timers.customCalibrateTimer > 50)
//      {
//        uiP->dspPacket.funcs2.led8 = true;
//        uiP->dspPacket.funcs2.led16 = true;
//        uiP->dspPacket.funcs2.led32 = true;
//      }
//      else if(uiP->timers.customCalibrateTimer > 25)
//      {
//        uiP->dspPacket.funcs2.led8 = true;
//        uiP->dspPacket.funcs2.led16 = true;
//        uiP->dspPacket.funcs2.led32 = true;
//        uiP->dspPacket.funcs2.ledCustom = true;
//      }
//      else /*if(uiP->timers.customCalibrateTimer > 25)*/
//      {       
//        uiP->dspPacket.funcs2.ledCustom = false;  
//        uiP->dspPacket.funcs2.led8 = true;
//      }
//      
//      if(!uiP->timers.customCalibrateTimer)
//        uiP->timers.customCalibrateTimer = ANIMATIONTIME;  
//    }
//  }   
//  else
//    uiP->dspPacket.funcs2.ledCustom = false;

    /*uiP->dspPacket.funcs.ledWater = (rfP->Dispenser.sets.state == _DispenserState_WaterPump) ? true : false;
    uiP->dspPacket.funcs.ledCubicIce = (rfP->Dispenser.sets.state == _DispenserState_IceCube) ? true : false;
    uiP->dspPacket.funcs.ledSplitIce = (rfP->Dispenser.sets.state == _DispenserState_IceCrash) ? true : false;
    uiP->dspPacket.funcs.ledDispenserSW = true;*/
}
void uiDspCabinIconLED(const tsUiProcConsts *consts, tsUiProcParams *params, unsigned char cabinIndex,
                       teDisplayLedState ledx)
{
    tsUiRam *uiP = params->ui;

    if ((cabinIndex < DSP_USED_CABINS_COUNT) && (ledx < eDisplayLedStateAnimation))
    {
        uiP->dspPacket.funcs.allFunctions[4] &= ~(1 << cabinIndex);  // first clear bit
        uiP->dspPacket.funcs.allFunctions[4] |= (ledx << cabinIndex); // then set if ledx is set
    }
}

void uiDspCabinIcon(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    const stRefrigerator_Flash *rfC = consts->rf;
    tsUiRam *uiP = params->ui;

    for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
    {
        if (rfC->Cabin[i].type != _CabinType_NONE)
        {
            if (i == uiP->dspValues.dspSelector)
                uiDspCabinIconLED(consts, params, i, eDisplayLedStateOn);
            else
                uiDspCabinIconLED(consts, params, i, eDisplayLedStateOff);
        }
    }
}

void uiDspDoorsAtQuick(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;

    /*
    if (params->rf->Cabin[0].components.doorSwitch[0].value ||
        params->rf->Cabin[0].components.doorSwitch[1].value)
    {
        uiDspCabinQuickLED(consts, params, 0, eDisplayLedStateAnimation);
        uiP->dspPacket.funcs.ledDispOff = true;
    }
    else
    {
        uiDspCabinQuickLED(consts, params, 0, eDisplayLedStateOff);
        uiP->dspPacket.funcs.ledDispOff = false;
    }

    if (params->rf->Cabin[1].components.doorSwitch[0].value ||
        params->rf->Cabin[1].components.doorSwitch[1].value)
        uiDspCabinQuickLED(consts, params, 1, eDisplayLedStateAnimation);
    else
        uiDspCabinQuickLED(consts, params, 1, eDisplayLedStateOff);
    */
    
    /*** APW : Door gap check in CT mode ***/
    if(refrigerator_R.ui.states.commercial.status == _UI_Commercial_None)       //Normal mode (Service only)
    {
      //FF is open door only.
      if (params->rf->Cabin[0].components.doorSwitch[0].value ||
           params->rf->Cabin[0].components.doorSwitch[1].value)
          uiDspCabinQuickLED(consts, params, 0, eDisplayLedStateAnimation);
      else
          uiDspCabinQuickLED(consts, params, 0, eDisplayLedStateOff);
      //FRZ is open door only.
      if (params->rf->Cabin[1].components.doorSwitch[0].value ||
           params->rf->Cabin[1].components.doorSwitch[1].value)
          uiDspCabinQuickLED(consts, params, 1, eDisplayLedStateAnimation);
      else
          uiDspCabinQuickLED(consts, params, 1, eDisplayLedStateOff);
      //FF and FRZ are close door.
      if (!(params->rf->Cabin[0].components.doorSwitch[0].value ||
            params->rf->Cabin[0].components.doorSwitch[1].value) &&
          !(params->rf->Cabin[1].components.doorSwitch[0].value ||
            params->rf->Cabin[1].components.doorSwitch[1].value))
          uiP->dspPacket.funcs.ledDispOff = true;
      else
          uiP->dspPacket.funcs.ledDispOff = false;
    }
    else                                                                        //Commercial mode
    {
      for (unsigned char i = 0; i < _NumOfCabin; i++)
      {
      if ((params->rf->Cabin[i].components.doorSwitch[0].value == _SwitchValue_OPEN) ||
            (params->rf->Cabin[i].components.doorSwitch[1].value == _SwitchValue_OPEN))
            uiDspCabinQuickLED(consts, params, i, eDisplayLedStateAnimation);
      else
        if(!params->rf->Cabin[i].sets.user.functions.Quick)
            uiDspCabinQuickLED(consts, params, i, eDisplayLedStateOff);
      }
    }
    /*** APW : Door gap check in CT mode ***/
}
