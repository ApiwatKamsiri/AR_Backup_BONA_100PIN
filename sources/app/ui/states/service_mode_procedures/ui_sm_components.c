#include "ui_sm_components.h"
#include "../../procedures/ui_slides.h"
#include "../../components/ui_buzzer.h"

#ifdef SERVICEMODEavailable

#define _SecureRotatorTime (5 * 50) //(sec*50) *20ms
#define _SecurePumpTime (5 * 50)    //(sec*50) *20ms
//#define _SecureValveTime	(5 * 50)    //(sec*50) *20ms
#define _SecureSelectorTime (1 * 50) //(sec*50) *20ms
#define _SecureShutterTime (1 * 50)  //(sec*50) *20ms

#define _Comp_Fan_TestOnTime (1.8 * 50)           // ( sec * 50 ) *20msec
#define _Comp_Fan_TestOffTime (0.2 * 50)          // ( sec * 50 ) *20msec
#define _Comp_Heater_TestOnTime (1.8 * 50)        // ( sec * 50 ) *20msec
#define _Comp_Heater_TestOffTime (0.2 * 50)       // ( sec * 50 ) *20msec
#define _Comp_Ionizer_TestOnTime (1.8 * 50)       // ( sec * 50 ) *20msec
#define _Comp_Ionizer_TestOffTime (0.2 * 50)      // ( sec * 50 ) *20msec
#define _Comp_Violed_TestOnTime (1.8 * 50)        // ( sec * 50 ) *20msec
#define _Comp_Violed_TestOffTime (0.2 * 50)       // ( sec * 50 ) *20msec
#define _Comp_Light_TestOnTime (1.8 * 50)         // ( sec * 50 ) *20msec
#define _Comp_Light_TestOffTime (0.2 * 50)        // ( sec * 50 ) *20msec
#define _Comp_Rotator_TestCWTime (0.9 * 50)       // ( sec * 50 ) *20msec
#define _Comp_Rotator_TestCCWTime (0.9 * 50)      // ( sec * 50 ) *20msec
#define _Comp_Rotator_TestOffTime (0.2 * 50)      // ( sec * 50 ) *20msec
#define _Comp_Mixer_TestOnTime (1.8 * 50)         // ( sec * 50 ) *20msec
#define _Comp_Mixer_TestOffTime (0.2 * 50)        // ( sec * 50 ) *20msec
#define _Comp_Pump_TestPumpTime (0.0 * 50)        // ( sec * 50 ) *20msec
#define _Comp_Pump_TestVacumTime (1.8 * 50)       // ( sec * 50 ) *20msec
#define _Comp_Pump_TestOffTime (0.2 * 50)         // ( sec * 50 ) *20msec
#define _Comp_Valve_TestOnTime (1.8 * 50)         // ( sec * 50 ) *20msec
#define _Comp_Valve_TestOffTime (0.2 * 50)        // ( sec * 50 ) *20msec
#define _Comp_Selector_TestSelectATime (0.9 * 50) // ( sec * 50 ) *20msec
#define _Comp_Selector_TestSelectBTime (0.9 * 50) // ( sec * 50 ) *20msec
#define _Comp_Selector_TestOffTime (0.2 * 50)     // ( sec * 50 ) *20msec
#define _Comp_Shutter_TestOpeningTime (0.9 * 50)  // ( sec * 50 ) *20msec
#define _Comp_Shutter_TestClosingTime (0.9 * 50)  // ( sec * 50 ) *20msec
#define _Comp_Shutter_TestOffTime (0.2 * 50)      // ( sec * 50 ) *20msec
#define _Comp_Pusher_TestOnTime (1.8 * 50)        // ( sec * 50 ) *20msec
#define _Comp_Pusher_TestOffTime (0.2 * 50)       // ( sec * 50 ) *20msec
#define _Comp_Compressor_TestOnTime (5.0 * 50)    // ( sec * 50 ) *20msec
#define _Comp_Compressor_TestOffTime (0.0 * 50)   // ( sec * 50 ) *20msec

#define _Comp_Fan_TestTime (_Comp_Fan_TestOnTime + \
                            _Comp_Fan_TestOffTime)
#define _Comp_Heater_TestTime (_Comp_Heater_TestOnTime + \
                               _Comp_Heater_TestOffTime)
#define _Comp_Ionizer_TestTime (_Comp_Ionizer_TestOnTime + \
                                _Comp_Ionizer_TestOffTime)
#define _Comp_Violed_TestTime (_Comp_Violed_TestOnTime + \
                               _Comp_Violed_TestOffTime)
#define _Comp_Light_TestTime (_Comp_Light_TestOnTime + \
                              _Comp_Light_TestOffTime)
#define _Comp_Rotator_TestTime (_Comp_Rotator_TestCWTime +  \
                                _Comp_Rotator_TestCCWTime + \
                                _Comp_Rotator_TestOffTime)
#define _Comp_Mixer_TestTime (_Comp_Mixer_TestOnTime + \
                              _Comp_Mixer_TestOffTime)
#define _Comp_Pump_TestTime (_Comp_Pump_TestPumpTime +  \
                             _Comp_Pump_TestVacumTime + \
                             _Comp_Pump_TestOffTime)
#define _Comp_Valve_TestTime (_Comp_Valve_TestOnTime + \
                              _Comp_Valve_TestOffTime)
#define _Comp_Selector_TestTime (_Comp_Selector_TestSelectATime + \
                                 _Comp_Selector_TestSelectBTime + \
                                 _Comp_Selector_TestOffTime)
#define _Comp_Shutter_TestTime (_Comp_Shutter_TestOpeningTime + \
                                _Comp_Shutter_TestClosingTime + \
                                _Comp_Shutter_TestOffTime)
#define _Comp_Pusher_TestTime (_Comp_Pusher_TestOnTime + \
                               _Comp_Pusher_TestOffTime)
#define _Comp_Compressor_TestTime (_Comp_Compressor_TestOnTime + \
                                   _Comp_Compressor_TestOffTime)

void ComponentsCheckCooler(tsUiProcParams *params, stCooler_Flash const *coolerC);
void ComponentsCheckCabin(tsUiProcParams *params, stCabin_Flash const *cabinC, unsigned char cabin_Count);
void ComponentsCheckIceMachine(tsUiProcParams *params, stIceMachine_Flash const *iceMachineC);
//void ComponentsCheckIceCreamMachine(tsUiProcParams *params, stIceCreamMachine_Flash const *iceCreamMachineC);
void ComponentsCheckWaterSource(tsUiProcParams *params, stWaterSource_Flash const *waterSourceC);
void ComponentsCheckIceSource(tsUiProcParams *params, stIceSource_Flash const *iceSourceC);

void ComponentsTestCompressor(stCompressor_Flash const *compressorC, stCompressor_Ram *compressorP, unsigned char *testTimer);
void ComponentsCTRLCompressor(const tsUiProcConsts *consts, tsUiProcParams *params, stCompressor_Flash const *compressorC, stCompressor_Ram *compressorP, bool change);
void ComponentsCTRLSolenoidValve(const tsUiProcConsts *consts, tsUiProcParams *params, stSolenoidValve_Flash const *solenoidValveC, stSolenoidValve_Ram *solenoidValveP, bool change);
void ComponentsTestFan(stFan_Flash const *fanC, stFan_Ram *fanP, unsigned char *testTimer);
void ComponentsCTRLFan(const tsUiProcConsts *consts, tsUiProcParams *params, stFan_Flash const *fanC, stFan_Ram *fanP, bool change);
void ComponentsCTRL_CondenserFan(const tsUiProcConsts *consts, tsUiProcParams *params, stFan_Flash const *fanC, stFan_Ram *fanP, stCoolerParameters_Flash const *param, bool change);
void ComponentsCTRLFlap(const tsUiProcConsts *consts, tsUiProcParams *params, stFlap_Flash const *flapC, stFlap_Ram *flapP, bool change);
void ComponentsTestHeater(stHeater_Flash const *heaterC, stHeater_Ram *heaterP, unsigned char *testTimer);
void ComponentsCTRLHeater(const tsUiProcConsts *consts, tsUiProcParams *params, stHeater_Flash const *heaterC, stHeater_Ram *heaterP, bool change);
void ComponentsTestIonizer(stIonizer_Flash const *ionizerC, stIonizer_Ram *ionizeP, unsigned char *testTimer);
void ComponentsCTRLIonizer(const tsUiProcConsts *consts, tsUiProcParams *params, stIonizer_Flash const *ionizerC, stIonizer_Ram *ionizerP, bool change);
void ComponentsTestVioled(stVioLed_Flash const *violedC, stVioLed_Ram *violedP, unsigned char *testTimer);
void ComponentsCTRLVioled(const tsUiProcConsts *consts, tsUiProcParams *params, stVioLed_Flash const *violedC, stVioLed_Ram *violedP, bool change);
void ComponentsTestLight(stLight_Flash const *lightC, stLight_Ram *lightP, unsigned char *testTimer);
void ComponentsCTRLLight(const tsUiProcConsts *consts, tsUiProcParams *params, stLight_Flash const *lightC, stLight_Ram *lightP, bool change);
void ComponentsCTRLHandleRGBLight(const tsUiProcConsts *consts, tsUiProcParams *params, stHandleRGBLight_Flash const *lightC, stHandleRGBLight_Ram *lightP, bool change);
void ComponentsTestRotator(stRotator_Flash const *rotatorC, stRotator_Ram *rotatorP, unsigned char *testTimer);
void ComponentsCTRLRotator(const tsUiProcConsts *consts, tsUiProcParams *params, stRotator_Flash const *rotatorC, stRotator_Ram *rotatorP, bool change);
void ComponentsTestMixer(stMixer_Flash const *mixerC, stMixer_Ram *mixerP, unsigned char *testTimer);
void ComponentsCTRLMixer(const tsUiProcConsts *consts, tsUiProcParams *params, stMixer_Flash const *mixerC, stMixer_Ram *mixerP, bool change);
void ComponentsTestPump(stPump_Flash const *pumpC, stPump_Ram *pumpP, unsigned char *testTimer);
void ComponentsCTRLPump(const tsUiProcConsts *consts, tsUiProcParams *params, stPump_Flash const *pumpC, stPump_Ram *pumpP, bool change);
void ComponentsTestValve(stValve_Flash const *valveC, stValve_Ram *valveP, unsigned char *testTimer);
void ComponentsCTRLValve(const tsUiProcConsts *consts, tsUiProcParams *params, stValve_Flash const *valveC, stValve_Ram *valveP, bool change);
void ComponentsTestSelector(stSelector_Flash const *selectorC, stSelector_Ram *selectorP, unsigned char *testTimer);
void ComponentsCTRLSelector(const tsUiProcConsts *consts, tsUiProcParams *params, stSelector_Flash const *selectorC, stSelector_Ram *selectorP, bool change);
void ComponentsTestShutter(stShutter_Flash const *shutterC, stShutter_Ram *shutterP, unsigned char *testTimer);
void ComponentsCTRLShutter(const tsUiProcConsts *consts, tsUiProcParams *params, stShutter_Flash const *shutterC, stShutter_Ram *shutterP, bool change);
void ComponentsTestPusher(stPusher_Flash const *pusherC, stPusher_Ram *pusherP, unsigned char *testTimer);
void ComponentsCTRLPusher(const tsUiProcConsts *consts, tsUiProcParams *params, stPusher_Flash const *pusherC, stPusher_Ram *pusherP, bool change);

void addValueFreqTable(tsUiSmCompsData *comps, unsigned char value);
unsigned char getFreqValueFromTable(tsUiSmCompsData *comps);

void uiSmCompsInit(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    comps->index = 0;
    comps->secureRotatorTimer = 0;
    comps->securePumpTimer = 0;
    // comps->secureValveTimer  = 0;
    comps->secureSelectorTimer = 0;
    comps->secureShutterTimer = 0;
    
    comps->currentIndex = 0;
    comps->lastFreqIndex = 1;
}

void uiSmComps20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    if (comps->testTimer)
        comps->testTimer--;
    if (comps->secureRotatorTimer)
        comps->secureRotatorTimer--;
    if (comps->securePumpTimer)
        comps->securePumpTimer--;
    //	if( comps->secureValveTimer ) comps->secureValveTimer--;
    if (comps->secureSelectorTimer)
        comps->secureSelectorTimer--;
    if (comps->secureShutterTimer)
        comps->secureShutterTimer--;
}

void uiSmCompsCheckAll(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    comps->number = 0;
    for (unsigned char i = 0; i < _SizeOfComponentsCheckTable; i++)
        comps->checkTable[i] = 0;

    ComponentsCheckCooler(params, &consts->rf->Cooler);
    ComponentsCheckCabin(params, &consts->rf->Cabin[0], _NumOfCabin);
   // ComponentsCheckIceCreamMachine(params, &consts->rf->IceCreamMachine);
    ComponentsCheckIceMachine(params, &consts->rf->IceMachine);
    ComponentsCheckWaterSource(params, &consts->rf->WaterSource);
    ComponentsCheckIceSource(params, &consts->rf->IceSource);

    for (unsigned char i = 0; i < _SizeOfComponentsCheckTable; i++)
    {
        if (comps->checkTable[i])
        {
            for (unsigned char j = 0; j < _NumOfCabin; j++)
            {
                if (comps->checkTable[i] & (0x01 << j))
                {
                    comps->table[comps->number][0] = i;
                    comps->table[comps->number][1] = j;
                    comps->number++;
                }
            }
        }
    }
}

void uiSmCompsCloseAll(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    // tsUiSmCompsData *comps = &params->ui->states.service.comps;
    UNUSED(consts);

    params->rf->Cooler.components.compressor.value = 0;
    params->rf->Cooler.components.condenserFan.speedOrPWM = 0;
    params->rf->Cooler.components.solenoidValve[0].position = 0;
    params->rf->IceMachine.components.caseRotator.value = _RotatorValue_OFF;
    params->rf->IceMachine.components.fan.speedOrPWM = 0;
   // params->rf->IceCreamMachine.components.mixer.value = _MixerValue_OFF;

    
    params->rf->WaterSource.components.mainPump.value = _PumpValue_OFF;
    params->rf->WaterSource.components.iceMachinePump.value = _PumpValue_OFF;
    params->rf->WaterSource.components.dispenserPump.value = _PumpValue_OFF;
    params->rf->WaterSource.components.mainValve.value = _ValveValue_CLOSE;
    params->rf->WaterSource.components.iceMachineValve.value = _ValveValue_CLOSE;
    params->rf->WaterSource.components.dispenserValve.value = _ValveValue_CLOSE;
    
    
    params->rf->IceSource.components.selector.value = _SelectorValue_OFF;
    params->rf->IceSource.components.shutter.value = _ShutterValue_OFF;
    params->rf->IceSource.components.pusher.value = _PusherValue_OFF;
    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        params->rf->Cabin[i].components.fan.speedOrPWM = 0;
        params->rf->Cabin[i].components.flap.value = 0;
        params->rf->Cabin[i].components.heaterA.value = _HeaterValue_OFF;
        params->rf->Cabin[i].components.heaterB.value = _HeaterValue_OFF;
        params->rf->Cabin[i].components.heaterC.value = _HeaterValue_OFF;
        params->rf->Cabin[i].components.ionizer.value = _IonizerValue_OFF;
        params->rf->Cabin[i].components.violed.value = _VioLedValue_OFF;
        params->rf->Cabin[i].components.blueLight.value = 0; // 0%
        // params->rf->Cabin[i].components.blueLight.step = BLUE_LIGHT_STEP;
        params->rf->Cabin[i].components.innerLight.value = 0; // 0%
        // params->rf->Cabin[i].components.innerLight.step = INNER_LIGHT_STEP;
    }
}

void uiSmCompsSecureAll(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;
    UNUSED(consts);

    if (!comps->secureRotatorTimer)
        params->rf->IceMachine.components.caseRotator.value = _RotatorValue_OFF;
    if (!comps->securePumpTimer)
    {
        params->rf->WaterSource.components.mainPump.value = _PumpValue_OFF;
        params->rf->WaterSource.components.dispenserPump.value = _PumpValue_OFF;
        params->rf->WaterSource.components.iceMachinePump.value = _PumpValue_OFF;
    }

    /*
  if( !comps->secureValveTimer ){
    params->rf->WaterSource.components.mainValve.value = _ValveValue_CLOSE;
    params->rf->WaterSource.components.dispenserValve.value = _ValveValue_CLOSE;
    params->rf->WaterSource.components.iceMachineValve.value = _ValveValue_CLOSE;
  }
*/
    if (!comps->secureSelectorTimer)
        params->rf->IceSource.components.selector.value = _SelectorValue_OFF;

    if (!comps->secureShutterTimer)
        params->rf->IceSource.components.shutter.value = _ShutterValue_OFF;
}

void uiSmCompsTestInit(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;
    UNUSED(consts);
    comps->index = 0;
    comps->testTimer = 0xFF;
}

void ComponentsFan(stFan_Flash const *fanC,
                   stFan_Ram *fanP,
                   bool isFanOn)
{
    unsigned short value;

    switch (fanC->type)
    {
    case _FanType_AC:
        // case _FanType_CDC:
        if (isFanOn)
            value = 1;
        else
            value = 0;
        break;
        
    case _FanType_DC:
        // case _FanType_CDC:
        if (isFanOn)
            value = 100;
        else
            value = 0;
        break;

    case _FanType_VDC:
        // case _FanType_CDC:
        if (isFanOn)
            value = 100;
        else
            value = 0;
        break;

    default:
        value = 0;
        break;
    }

    fanP->speedOrPWM = value;
}

void ComponentsLight(stLight_Flash const *lightC,
                     stLight_Ram *lightP,
                     bool isLightOn)
{
    unsigned short value;
    switch (lightC->type)
    {
    case _LightType_AC:
    case _LightType_DC:
    case _LightType_PWM:
        if (isLightOn)
            value = 100; // 10000;
        else
            value = 0;
        break;

    default:
        value = 0;
        break;
    }
    lightP->value = value;
    // lightP->step = INNER_LIGHT_STEP; // 0xFF;
}
void ComponentsHandleRGBLight(stHandleRGBLight_Flash const *handleLightC,
                     stHandleRGBLight_Ram *handleLightP,
                     bool isLightOn)
{
    unsigned short value;
    switch (handleLightC->type)
    {
    case _LightType_AC:
    case _LightType_DC:
    case _LightType_PWM:
        if (isLightOn)
            value = 100; // 10000;
        else
            value = 0;
        break;

    default:
        value = 0;
        break;
    }
    handleLightP->R_value = value;
    handleLightP->G_value = value;
    handleLightP->B_value = value;
    // lightP->step = INNER_LIGHT_STEP; // 0xFF;
}

void ComponentsVioled(stVioLed_Flash const *violedC,
                      stVioLed_Ram *violedP,
                      bool isVioledOn)
{
    /*enVioLedValues*/ unsigned short value;
    switch (violedC->type)
    {
    case _VioLedType_DC:
        // case _VioledType_PWM:
        if (isVioledOn)
            value = _VioLedValue_ON;
        else
            value = _VioLedValue_OFF;
        break;
        /*
                case _VioledType_PWM:
                    if (isVioledOn)
                        value = 10000;
                    else
                        value = _VioLedValue_OFF;
                break;
        */
    default:
        value = _VioLedValue_OFF;
        break;
    }
    violedP->value = value;
}

void uiSmCompsControlSpecials(const tsUiProcConsts *consts, tsUiProcParams *params, bool isSpecialsOn)
{
    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        ComponentsFan(&consts->rf->Cabin[i].components.fan,
                      &params->rf->Cabin[i].components.fan,
                      isSpecialsOn);
        ComponentsLight(&consts->rf->Cabin[i].components.innerLight,
                        &params->rf->Cabin[i].components.innerLight,
                        isSpecialsOn);
        ComponentsLight(&consts->rf->Cabin[i].components.blueLight,
                        &params->rf->Cabin[i].components.blueLight,
                        isSpecialsOn);
        ComponentsHandleRGBLight(&consts->rf->Cabin[i].components.handleLight,
                        &params->rf->Cabin[i].components.handleLight,
                        isSpecialsOn);
        ComponentsVioled(&consts->rf->Cabin[i].components.violed,
                         &params->rf->Cabin[i].components.violed,
                         isSpecialsOn);
    }

    ComponentsFan(&consts->rf->Cooler.components.condenserFan,
                  &params->rf->Cooler.components.condenserFan,
                  isSpecialsOn);
    /*
  ComponentsFan( &consts->rf->IceMachine.components.fan,
                 &params->rf->IceMachine.components.fan,
                 isSpecialsOn );
  */
}

void uiSmCompsControlSelected(const tsUiProcConsts *consts, tsUiProcParams *params, bool change)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    if (comps->index >= comps->number)
    {
        params->ui->slides.slideNumber = 0;
        return;
    }

    unsigned char compType = comps->table[comps->index][0];
    unsigned char compIndex = comps->table[comps->index][1];
     
    switch (compType)
    {
    case _Comp_Cooler_Compressor:
        uiSlideData(&params->ui->slides, eDisplaySpecialsCoolerCompressor, 0, eDisplayCabinDataTypeSpecial);
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLCompressor(consts, params,
                                 &consts->rf->Cooler.components.compressor,
                                 &params->rf->Cooler.components.compressor,
                                 change);
        break;

    case _Comp_Cooler_CondenserFan:
        uiSlideData(&params->ui->slides, eDisplaySpecialsCoolerCondenserFan, 0, eDisplayCabinDataTypeSpecial);
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLFan(consts, params,
                          &consts->rf->Cooler.components.condenserFan,
                          &params->rf->Cooler.components.condenserFan,
                          change);
        break;

    case _Comp_Cooler_SolenoidValve0:
        uiSlideData(&params->ui->slides, eDisplaySpecialsCoolerSolenoidValve, 0, eDisplayCabinDataTypeSpecial);
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLSolenoidValve(consts, params,
                                    &consts->rf->Cooler.components.solenoidValve[0],
                                    &params->rf->Cooler.components.solenoidValve[0],
                                    change);
        break;

    case _Comp_Cooler_SolenoidValve1:
        uiSlideData(&params->ui->slides, eDisplaySpecialsCoolerSolenoidValve, 0, eDisplayCabinDataTypeSpecial);
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLSolenoidValve(consts, params,
                                    &consts->rf->Cooler.components.solenoidValve[1],
                                    &params->rf->Cooler.components.solenoidValve[1],
                                    change);
        break;

    case _Comp_FF_Fan:
        uiSlideData(&params->ui->slides, eDisplaySpecialsFFFan, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;

        ComponentsCTRLFan(consts, params,
                          &consts->rf->Cabin[compIndex].components.fan,
                          &params->rf->Cabin[compIndex].components.fan,
                          change);
        break;

    case _Comp_FF_Flap:
        uiSlideData(&params->ui->slides, eDisplaySpecialsFFFlap, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;
        ComponentsCTRLFlap(consts, params,
                           &consts->rf->Cabin[compIndex].components.flap,
                           &params->rf->Cabin[compIndex].components.flap,
                           change);
        break;

    case _Comp_FF_LowTempHeater:
        uiSlideData(&params->ui->slides, eDisplaySpecialsFFLowTempHeater, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;
        ComponentsCTRLHeater(consts, params,
                             &consts->rf->Cabin[compIndex].components.heaterA,
                             &params->rf->Cabin[compIndex].components.heaterA,
                             change);
        break;

    case _Comp_FF_Ionizer:
        uiSlideData(&params->ui->slides, eDisplaySpecialsAllIonizer, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;
        ComponentsCTRLIonizer(consts, params,
                              &consts->rf->Cabin[compIndex].components.ionizer,
                              &params->rf->Cabin[compIndex].components.ionizer,
                              change);
        break;

    case _Comp_FF_Violed:
        uiSlideData(&params->ui->slides, eDisplaySpecialsAllVioled, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;
        ComponentsCTRLVioled(consts, params,
                             &consts->rf->Cabin[compIndex].components.violed,
                             &params->rf->Cabin[compIndex].components.violed,
                             change);
        break;

    case _Comp_FF_BlueLight:
        uiSlideData(&params->ui->slides, eDisplaySpecialsAllBlueLight, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;
        ComponentsCTRLLight(consts, params,
                            &consts->rf->Cabin[compIndex].components.blueLight,
                            &params->rf->Cabin[compIndex].components.blueLight,
                            change);
        // params->rf->Cabin[compIndex].components.blueLight.step = BLUE_LIGHT_STEP;
        break;

    case _Comp_FF_InnerLight:
        uiSlideData(&params->ui->slides, eDisplaySpecialsFFInnerLight, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);

        params->ui->slides.slideNumber = 3;
        ComponentsCTRLLight(consts, params,
                            &consts->rf->Cabin[compIndex].components.innerLight,
                            &params->rf->Cabin[compIndex].components.innerLight,
                            change);
        // params->rf->Cabin[compIndex].components.innerLight.step = INNER_LIGHT_STEP;

        break;
        
    case _Comp_FF_HandleLight:
      uiSlideData(&params->ui->slides, eDisplaySpecialsAllHandleLight, 0, eDisplayCabinDataTypeSpecial);
      uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
      uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
      
      params->ui->slides.slideNumber = 3;
      ComponentsCTRLHandleRGBLight(consts, params,
                          &consts->rf->Cabin[compIndex].components.handleLight,
                          &params->rf->Cabin[compIndex].components.handleLight,
                          change);
      
      break;

    case _Comp_FRZ_Fan:
        uiSlideData(&params->ui->slides, eDisplaySpecialsFRZFan, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;
        ComponentsCTRLFan(consts, params,
                          &consts->rf->Cabin[compIndex].components.fan,
                          &params->rf->Cabin[compIndex].components.fan,
                          change);
        break;

    case _Comp_FRZ_DefrostHeater:
        uiSlideData(&params->ui->slides, eDisplaySpecialsFRZDefrostHeater, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;
        ComponentsCTRLHeater(consts, params,
                             &consts->rf->Cabin[compIndex].components.heaterB,
                             &params->rf->Cabin[compIndex].components.heaterB,
                             change);
        break;

    case _Comp_FRZ_InnerLight:
        uiSlideData(&params->ui->slides, eDisplaySpecialsFRZInnerLight, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;
        ComponentsCTRLLight(consts, params,
                            &consts->rf->Cabin[compIndex].components.innerLight,
                            &params->rf->Cabin[compIndex].components.innerLight,
                            change);
        // params->rf->Cabin[compIndex].components.innerLight.step = INNER_LIGHT_STEP;
        break;

    case _Comp_JKR_Fan:
        uiSlideData(&params->ui->slides, eDisplaySpecialsJKRFan, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;
        ComponentsCTRLFan(consts, params,
                          &consts->rf->Cabin[compIndex].components.fan,
                          &params->rf->Cabin[compIndex].components.fan,
                          change);
        break;

    case _Comp_JKR_LowTempHeater:
        uiSlideData(&params->ui->slides, eDisplaySpecialsJKRLowTempHeater, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;
        ComponentsCTRLHeater(consts, params,
                             &consts->rf->Cabin[compIndex].components.heaterA,
                             &params->rf->Cabin[compIndex].components.heaterA,
                             change);
        break;

    case _Comp_JKR_DefrostHeater:
        uiSlideData(&params->ui->slides, eDisplaySpecialsJKRDefrostHeater, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;
        ComponentsCTRLHeater(consts, params,
                             &consts->rf->Cabin[compIndex].components.heaterB,
                             &params->rf->Cabin[compIndex].components.heaterB,
                             change);
        break;

    case _Comp_JKR_InnerLight:
        uiSlideData(&params->ui->slides, eDisplaySpecialsJKRInnerLight, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;
        ComponentsCTRLLight(consts, params,
                            &consts->rf->Cabin[compIndex].components.innerLight,
                            &params->rf->Cabin[compIndex].components.innerLight,
                            change);
        // params->rf->Cabin[compIndex].components.innerLight.step = INNER_LIGHT_STEP;
        break;

    case _Comp_FLAP_Fan:
        // Have to be defined
        break;

    case _Comp_FLAP_Flap:
        // Have to be defined
        break;

    case _Comp_FLAP_LowTempHeater:
        // Have to be defined
        break;

    case _Comp_FLAP_FlapHeater:
        // Have to be defined
        break;

    case _Comp_FLAP_ChannelHeater:
        // Have to be defined
        break;

    case _Comp_FLAP_InnerLight:
        // Have to be defined
        break;

        /*case _Comp_BioFresh_Fan:
            // Have to be defined
        break;

        case _Comp_BioFresh_InnerLight:
            // Have to be defined
        break;*/

    case _Comp_IceMachine_CaseRotator:
        uiSlideData(&params->ui->slides, eDisplaySpecialsIceMachineIceMotor, 0, eDisplayCabinDataTypeSpecial);
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLRotator(consts, params,
                              &consts->rf->IceMachine.components.caseRotator,
                              &params->rf->IceMachine.components.caseRotator,
                              change);
        break;

        
    case _Comp_IceMachine_Fan:
        uiSlideData(&params->ui->slides,  eDisplaySpecialsBioFreshFan, 0, eDisplayCabinDataTypeSpecial );
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLFan(consts, params,
                             &consts->rf->IceMachine.components.fan,
                             &params->rf->IceMachine.components.fan,
                             change);
        break;
        

  /*  case _Comp_IceCreamMachine_Mixer:
        uiSlideData(&params->ui->slides, eDisplaySpecialsIceCreamMachineMixer, 0, eDisplayCabinDataTypeSpecial);
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLMixer(consts, params,
                            &consts->rf->IceCreamMachine.components.mixer,
                            &params->rf->IceCreamMachine.components.mixer,
                            change);
        break;*/

    case _Comp_WaterSource_MainPump:
        uiSlideData(&params->ui->slides, eDisplaySpecialsWaterSourceMainPump, 0, eDisplayCabinDataTypeSpecial);
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLPump(consts, params,
                           &consts->rf->WaterSource.components.mainPump,
                           &params->rf->WaterSource.components.mainPump,
                           change);
        break;
    case _Comp_WaterSource_DispenserPump:
      uiSlideData(&params->ui->slides, eDisplaySpecialsWaterSourceDispenserValve, 0, eDisplayCabinDataTypeSpecial);
      params->ui->slides.slideNumber = 1;
      ComponentsCTRLPump(consts, params,
                         &consts->rf->WaterSource.components.dispenserPump,
                         &params->rf->WaterSource.components.dispenserPump,
                         change);
      break;
    case _Comp_WaterSource_IceMachinePump:
        uiSlideData(&params->ui->slides, eDisplaySpecialsWaterSourceIceMachineValve, 0, eDisplayCabinDataTypeSpecial);
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLPump(consts, params,
                           &consts->rf->WaterSource.components.iceMachinePump,
                           &params->rf->WaterSource.components.iceMachinePump,
                           change);
        break;

    case _Comp_WaterSource_MainValve:
        uiSlideData(&params->ui->slides, eDisplaySpecialsWaterSourceMainValve, 0, eDisplayCabinDataTypeSpecial);
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLValve(consts, params,
                            &consts->rf->WaterSource.components.mainValve,
                            &params->rf->WaterSource.components.mainValve,
                            change);
        break;

    case _Comp_WaterSource_DispenserValve:
    
        uiSlideData(&params->ui->slides, eDisplaySpecialsWaterSourceDispenserValve, 0, eDisplayCabinDataTypeSpecial);
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLValve(consts, params,
                            &consts->rf->WaterSource.components.dispenserValve,
                            &params->rf->WaterSource.components.dispenserValve,
                            change);
        break;

    case _Comp_WaterSource_IceMachineValve:

        uiSlideData(&params->ui->slides, eDisplaySpecialsWaterSourceIceMachineValve, 0, eDisplayCabinDataTypeSpecial);
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLValve(consts, params,
                            &consts->rf->WaterSource.components.iceMachineValve,
                            &params->rf->WaterSource.components.iceMachineValve,
                            change);
        break;

    case _Comp_WaterSource_WaterVentilHeater:
        uiSlideData(&params->ui->slides, eDisplaySpecialsWaterSourceWaterVentilHeater, 0, eDisplayCabinDataTypeSpecial);
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLHeater(consts, params,
                             &consts->rf->WaterSource.components.waterVentilHeater,
                             &params->rf->WaterSource.components.waterVentilHeater,
                             change);
        break;

    case _Comp_IceSource_Selector:
        uiSlideData(&params->ui->slides, eDisplaySpecialsIceSourceSelector, 0, eDisplayCabinDataTypeSpecial);
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLSelector(consts, params,
                               &consts->rf->IceSource.components.selector,
                               &params->rf->IceSource.components.selector,
                               change);
        break;

    case _Comp_IceSource_Shutter:
        uiSlideData(&params->ui->slides, eDisplaySpecialsIceSourceShutter, 0, eDisplayCabinDataTypeSpecial);
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLShutter(consts, params,
                              &consts->rf->IceSource.components.shutter,
                              &params->rf->IceSource.components.shutter,
                              change);
        break;

    case _Comp_IceSource_Pusher:
        uiSlideData(&params->ui->slides, eDisplaySpecialsIceSourcePusher, 0, eDisplayCabinDataTypeSpecial);
        params->ui->slides.slideNumber = 1;
        ComponentsCTRLPusher(consts, params,
                             &consts->rf->IceSource.components.pusher,
                             &params->rf->IceSource.components.pusher,
                             change);
        break;

    case _Comp_FRZ_HeaterA:
        uiSlideData(&params->ui->slides, eDisplaySpecialsAllHeaterA, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;
        ComponentsCTRLHeater(consts, params,
                             &consts->rf->Cabin[compIndex].components.heaterA,
                             &params->rf->Cabin[compIndex].components.heaterA,
                             change);
        break;

    case _Comp_FF_HeaterB:
        uiSlideData(&params->ui->slides, eDisplaySpecialsAllHeaterB, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;
        ComponentsCTRLHeater(consts, params,
                             &consts->rf->Cabin[compIndex].components.heaterB,
                             &params->rf->Cabin[compIndex].components.heaterB,
                             change);
        break;

    case _Comp_FF_HeaterC:
    case _Comp_FRZ_HeaterC:
    case _Comp_JKR_HeaterC:
        uiSlideData(&params->ui->slides, eDisplaySpecialsAllHeaterC, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&params->ui->slides, compIndex, 2, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 3;
        ComponentsCTRLHeater(consts, params,
                             &consts->rf->Cabin[compIndex].components.heaterC,
                             &params->rf->Cabin[compIndex].components.heaterC,
                             change);
        break;

    default:;
        break;
    }
}

void ComponentsCheckCooler(tsUiProcParams *params, stCooler_Flash const *coolerC)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    if (coolerC->type != _CoolerType_NONE)
    {
        if (coolerC->components.compressor.type)
            comps->checkTable[_Comp_Cooler_Compressor] = 1;

        if (coolerC->components.condenserFan.type)
            comps->checkTable[_Comp_Cooler_CondenserFan] = 1;

        if (coolerC->components.solenoidValve[0].type)
            comps->checkTable[_Comp_Cooler_SolenoidValve0] = 1;

        if (coolerC->components.solenoidValve[1].type)
            comps->checkTable[_Comp_Cooler_SolenoidValve1] = 1;
    }
}

void ComponentsCheckCabin(tsUiProcParams *params, stCabin_Flash const *cabinC,
                          unsigned char cabin_Count)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    for (unsigned char i = 0; i < cabin_Count; i++)
    {
        switch (cabinC[i].type)
        {
        case _CabinType_FF:
            if (cabinC[i].components.fan.type)
                comps->checkTable[_Comp_FF_Fan] |= 0x01 << i;
            if (cabinC[i].components.flap.type)
                comps->checkTable[_Comp_FF_Flap] |= 0x01 << i;
            if (cabinC[i].components.heaterA.type)
                comps->checkTable[_Comp_FF_LowTempHeater] |= 0x01 << i;
            if (cabinC[i].components.heaterB.type)
                comps->checkTable[_Comp_FF_HeaterB] |= 0x01 << i;
            if (cabinC[i].components.heaterC.type)
                comps->checkTable[_Comp_FF_HeaterC] |= 0x01 << i;
            if (cabinC[i].components.ionizer.type)
                comps->checkTable[_Comp_FF_Ionizer] |= 0x01 << i;
            if (cabinC[i].components.violed.type)
                comps->checkTable[_Comp_FF_Violed] |= 0x01 << i;
            if (cabinC[i].components.blueLight.type)
                comps->checkTable[_Comp_FF_BlueLight] |= 0x01 << i;
            if (cabinC[i].components.innerLight.type)
                comps->checkTable[_Comp_FF_InnerLight] |= 0x01 << i;
            if (cabinC[i].components.handleLight.type)
              comps->checkTable[_Comp_FF_HandleLight] |= 0x01 << i;
            break;

        case _CabinType_FRZ:
            if (cabinC[i].components.fan.type)
                comps->checkTable[_Comp_FRZ_Fan] |= 0x01 << i;
            if (cabinC[i].components.heaterA.type)
                comps->checkTable[_Comp_FRZ_HeaterA] |= 0x01 << i;
            if (cabinC[i].components.heaterB.type)
                comps->checkTable[_Comp_FRZ_DefrostHeater] |= 0x01 << i;
            if (cabinC[i].components.heaterC.type)
                comps->checkTable[_Comp_FRZ_HeaterC] |= 0x01 << i;
            if (cabinC[i].components.innerLight.type)
                comps->checkTable[_Comp_FRZ_InnerLight] |= 0x01 << i;
            break;

        case _CabinType_JKR:
            if (cabinC[i].components.fan.type)
                comps->checkTable[_Comp_JKR_Fan] |= 0x01 << i;
            if (cabinC[i].components.heaterA.type)
                comps->checkTable[_Comp_JKR_LowTempHeater] |= 0x01 << i;
            if (cabinC[i].components.heaterB.type)
                comps->checkTable[_Comp_JKR_DefrostHeater] |= 0x01 << i;
            if (cabinC[i].components.heaterC.type)
                comps->checkTable[_Comp_JKR_HeaterC] |= 0x01 << i;
            if (cabinC[i].components.innerLight.type)
                comps->checkTable[_Comp_JKR_InnerLight] |= 0x01 << i;
            break;
        case _CabinType_FLAP:
            if (cabinC[i].components.fan.type)
                comps->checkTable[_Comp_FLAP_Fan] |= 0x01 << i;
            if (cabinC[i].components.flap.type)
                comps->checkTable[_Comp_FLAP_Flap] |= 0x01 << i;
            if (cabinC[i].components.heaterA.type)
                comps->checkTable[_Comp_FLAP_LowTempHeater] |= 0x01 << i;
            if (cabinC[i].components.heaterB.type)
                comps->checkTable[_Comp_FLAP_FlapHeater] |= 0x01 << i;
            if (cabinC[i].components.heaterC.type)
                comps->checkTable[_Comp_FLAP_ChannelHeater] |= 0x01 << i;
            if (cabinC[i].components.innerLight.type)
                comps->checkTable[_Comp_FLAP_InnerLight] |= 0x01 << i;
            break;
            /*case _CabinType_BioFRESH:
                if (cabinC[i].components.fan.type)
                    comps->checkTable[_Comp_BioFresh_Fan] |= 0x01 << i;
                if (cabinC[i].components.innerLight.type)
                    comps->checkTable[_Comp_BioFresh_InnerLight] |= 0x01 << i;
            break;*/

        case _CabinType_NONE:
        default:;
            break;
        }
    }
}

void ComponentsCheckIceMachine(tsUiProcParams *params, stIceMachine_Flash const *iceMachineC)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    if (iceMachineC->type != _IceMachineType_NONE)
    {
        if (iceMachineC->components.caseRotator.type)
            comps->checkTable[_Comp_IceMachine_CaseRotator] = 1;
        if (iceMachineC->components.fan.type)
          comps->checkTable[_Comp_IceMachine_Fan] = 1;
    }
}

/*void ComponentsCheckIceCreamMachine(tsUiProcParams *params, stIceCreamMachine_Flash const *iceCreamMachineC)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    if (iceCreamMachineC->type != _IceCreamMachineType_NONE)
    {
        if (iceCreamMachineC->components.mixer.type)
            comps->checkTable[_Comp_IceCreamMachine_Mixer] = 1;
    }
}*/

void ComponentsCheckWaterSource(tsUiProcParams *params, stWaterSource_Flash const *waterSourceC)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    if (waterSourceC->type.supply != _WaterSupplyType_NONE)
    {
        if (waterSourceC->components.mainPump.type)
          comps->checkTable[_Comp_WaterSource_MainPump] = 1;
        if (waterSourceC->components.dispenserPump.type)
          comps->checkTable[_Comp_WaterSource_DispenserPump] = 1;
        if (waterSourceC->components.iceMachinePump.type)
          comps->checkTable[_Comp_WaterSource_IceMachinePump] = 1;    
        if (waterSourceC->components.mainValve.type)
            comps->checkTable[_Comp_WaterSource_MainValve] = 1;
        if (waterSourceC->components.dispenserValve.type)
            comps->checkTable[_Comp_WaterSource_DispenserValve] = 1;
        if (waterSourceC->components.iceMachineValve.type)
            comps->checkTable[_Comp_WaterSource_IceMachineValve] = 1;
        if (waterSourceC->components.waterVentilHeater.type)
            comps->checkTable[_Comp_WaterSource_WaterVentilHeater] = 1;
    }
}

void ComponentsCheckIceSource(tsUiProcParams *params, stIceSource_Flash const *iceSourceC)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    if (iceSourceC->type != _IceSourceType_NONE)
    {
        if (iceSourceC->components.selector.type)
            comps->checkTable[_Comp_IceSource_Selector] = 1;
        if (iceSourceC->components.shutter.type)
            comps->checkTable[_Comp_IceSource_Shutter] = 1;
        if (iceSourceC->components.pusher.type)
            comps->checkTable[_Comp_IceSource_Pusher] = 1;
    }
}

void ComponentsTestCompressor(stCompressor_Flash const *compressorC,
                              stCompressor_Ram *compressorP,
                              unsigned char *testTimer)
{
    unsigned char value = 0;
    switch (compressorC->type)
    {
    case _CompressorType_ACOut:
    case _CompressorType_VCC_PANASONIC:
    case _CompressorType_UART:
        // case _CompressorType_VCC_EMBRACO_VEMX:
        // case _CompressorType_VCC_EKI:
        if (*testTimer > _Comp_Compressor_TestTime)
            *testTimer = _Comp_Compressor_TestTime;

        if (*testTimer > _Comp_Compressor_TestOffTime)
            value = 100;
        else
            value = 0;
        break;
    }
    compressorP->value = value;
}

void addValueFreqTable(tsUiSmCompsData *comps, unsigned char value)
{

    if (comps->lastFreqIndex >= 30)
        return;

    for (unsigned char i = 0; i < comps->lastFreqIndex; i++)
    {
        if (value == comps->freqTable[i])
            return;
    }

    comps->freqTable[comps->lastFreqIndex] = value;

    if (comps->lastFreqIndex < 30)
        comps->lastFreqIndex++;
}

unsigned char getFreqValueFromTable(tsUiSmCompsData *comps)
{

    comps->currentIndex++;
    if (comps->currentIndex >= comps->lastFreqIndex)
        comps->currentIndex = 0;

    return comps->freqTable[comps->currentIndex];
}

void uiSmCompsInitVCCFreqTable(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    for (unsigned char i = 0; i < 30; i++)
        comps->freqTable[i] = 0;

    comps->currentIndex = 0;
    comps->lastFreqIndex = 1;

    enCoolerTypes coolerType = consts->rf->Cooler.type;

    if (consts->rf->Cooler.components.ambientTempSensor.type != _TempSensorType_NONE)
    {
        for (unsigned char i = 0; i < _AmbientSize; i++)
        {
            for (unsigned char j = 0; j < _LevelSize; j++)
            {
                addValueFreqTable(comps, consts->rf->Cooler.parameters.compressor.FF.speedLevel[i][j]);
                if (coolerType != _CoolerType_DEFAULT)
                    addValueFreqTable(comps, consts->rf->Cooler.parameters.compressor.FRZ.speedLevel[i][j]);
            }
        }
    }
    else
    {
        for (unsigned char i = 0; i < 9; i++)
        {
            addValueFreqTable(comps, consts->rf->Cooler.parameters.compressor.speedLevelFF[i]);
            if (coolerType != _CoolerType_DEFAULT)
                addValueFreqTable(comps, consts->rf->Cooler.parameters.compressor.speedLevelFRZ[i]);
        }
    }
}

void ComponentsCTRLCompressor(const tsUiProcConsts *consts, tsUiProcParams *params,
                              stCompressor_Flash const *compressorC,
                              stCompressor_Ram *compressorP,
                              bool change)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    unsigned char value = compressorP->value;

    switch (compressorC->type)
    {
    case _CompressorType_ACOut:
        if (change)
        {
            if (value)
                value = 0;
            else
                value = 1;
        }

        if (value)
            uiSlideData(&params->ui->slides, eDisplaySpecialsOn, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        else
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;

    case _CompressorType_VCC_PANASONIC:
        // case _CompressorType_VCC_EMBRACO_VEMX:
    //case _CompressorType_VCC_EKI:
    case _CompressorType_UART:

        if (change)
        {
            value = getFreqValueFromTable(comps);
            sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
        }
        if (value)
        {
            if (value / 100)
            {

                if (params->ui->slides.slideIndex != params->ui->slides.slideNumber)
                {
                    if (params->ui->states.service.timer > 50)
                        uiSlideData(&params->ui->slides, value / 100, params->ui->slides.slideNumber, eDisplayCabinDataTypeNumber);
                    else
                        uiSlideData(&params->ui->slides, value % 100, params->ui->slides.slideNumber, eDisplayCabinDataTypeNumber);
                }
                else
                    uiSlideData(&params->ui->slides, value % 100, params->ui->slides.slideNumber, eDisplayCabinDataTypeNumber);
            }
            else
                uiSlideData(&params->ui->slides, value, params->ui->slides.slideNumber, eDisplayCabinDataTypeNumber);
        }
        else
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;
    }
    compressorP->value = value;
    params->ui->slides.slideNumber++;
}

void ComponentsCTRLSolenoidValve(const tsUiProcConsts *consts, tsUiProcParams *params,
                                 stSolenoidValve_Flash const *solenoidValveC,
                                 stSolenoidValve_Ram *solenoidValveP,
                                 bool change)
{
    unsigned char positionIndex = solenoidValveP->positionIndex;
    solenoidValveP->testMode = 0;

    switch (solenoidValveC->type)
    {
    case _SolenoidValveType_StepMotor:
        if (change)
        {
            positionIndex++;
            sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
            if (positionIndex >= solenoidValveC->positionsSize)
                positionIndex = 0;
        }
        uiSlideData(&params->ui->slides, positionIndex, params->ui->slides.slideNumber, eDisplayCabinDataTypeNumber);
        break;
    }
    solenoidValveP->positionIndex = positionIndex;
    params->ui->slides.slideNumber++;
}

void ComponentsTestFan(stFan_Flash const *fanC,
                       stFan_Ram *fanP,
                       unsigned char *testTimer)
{
    unsigned short value = 0;
    switch (fanC->type)
        {
        case _FanType_AC:
        case _FanType_CDC:
            // case _FanType_VDC:
            if (*testTimer > _Comp_Fan_TestTime)
                *testTimer = _Comp_Fan_TestTime;

            if (*testTimer > _Comp_Fan_TestOffTime)
                value = 100;
            else
                value = 0;
            break;
        case _FanType_DC:
            // case _FanType_CDC:
            if (*testTimer > _Comp_Fan_TestTime)
                *testTimer = _Comp_Fan_TestTime;

            if (*testTimer > _Comp_Fan_TestOffTime)
                value = 100;
            else
                value = 0;
            break;
        }
        fanP->speedOrPWM = value;
}

void addValueFanTable(stFanTable *ft, unsigned char value)
{

    if (ft->lastIndex >= FAN_TABLE_SIZE)
        return;

    for (unsigned char i = 0; i < ft->lastIndex; i++)
    {
        if (value == ft->table[i])
            return;
    }

    ft->table[ft->lastIndex] = value;

    if (ft->lastIndex < FAN_TABLE_SIZE)
        ft->lastIndex++;
}

unsigned char getValueFromTable(stFanTable *ft)
{

    ft->currentIndex++;
    if (ft->currentIndex >= ft->lastIndex)
        ft->currentIndex = 0;

    return ft->table[ft->currentIndex];
}

void uiSmCompsSetFansCurrentIndexToInit(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    stFanTable *fts = params->ui->states.service.comps.fanTables;

    for (unsigned char i = 0; i < NUM_OF_FANS; i++)
        fts[i].currentIndex = 0;
}

void uiSmCompsSetFansCurrentIndexToLastIndex(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    stFanTable *fts = params->ui->states.service.comps.fanTables;

    for (unsigned char i = 0; i < NUM_OF_FANS; i++)
        fts[i].currentIndex = (fts[i].lastIndex - 1);
}

void uiSmCompsInitFanTables(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    stFanTable *fts = params->ui->states.service.comps.fanTables;

    for (unsigned char i = 0; i < NUM_OF_FANS; i++)
    {
        for (unsigned char j = 0; j < FAN_TABLE_SIZE; j++)
            fts[i].table[j] = 0;

        fts[i].currentIndex = 0;
        fts[i].lastIndex = 1;
    }

    enCoolerTypes coolerType = consts->rf->Cooler.type;
    unsigned char ambientSensor = 0;

    if (consts->rf->Cooler.components.ambientTempSensor.type)
        ambientSensor = 1;
    else
        ambientSensor = 0;

    for (unsigned char i = 0; i < NUM_OF_FANS; i++)
    {
        if (i == 4) // condanser fan
        {

            if (consts->rf->Cooler.components.condenserFan.type == _FanType_VDC)
            {

                if (ambientSensor)
                {

                    for (unsigned char j = 0; j < _AmbientSize; j++)
                    {
                        for (unsigned char k = 0; k < _LevelSize; k++)
                        {
                            addValueFanTable(&fts[i], consts->rf->Cooler.parameters.condenserFan.FF.speedLevel[j][k]);
                            if (coolerType != _CoolerType_DEFAULT)
                                addValueFanTable(&fts[i], consts->rf->Cooler.parameters.condenserFan.FRZ.speedLevel[j][k]);
                        }
                    }
                }
                else
                {
                    for (unsigned char j = 0; j < 9; j++)
                    {
                        addValueFanTable(&fts[i], consts->rf->Cooler.parameters.condenserFan.speedLevelFF[j]);
                        if (coolerType != _CoolerType_DEFAULT)
                            addValueFanTable(&fts[i], consts->rf->Cooler.parameters.condenserFan.speedLevelFRZ[j]);
                    }
                }
            }
        }
        else // cabins fan
        {

            if (consts->rf->Cabin[i].components.fan.type == _FanType_VDC)
            {

                if (ambientSensor)
                {

                    for (unsigned char j = 0; j < _AmbientSize; j++)
                    {
                        for (unsigned char k = 0; k < _LevelSize; k++)
                        {
                            addValueFanTable(&fts[i], consts->rf->Cabin[i].parameters.Fan.AmbientSpeedLevel[j][k]);
                        }
                    }
                }
                else
                {
                    for (unsigned char j = 0; j < 9; j++)
                    {
                        addValueFanTable(&fts[i], consts->rf->Cabin[i].parameters.Fan.SpeedLevel[j]);
                    }
                }
            }
        }
    }
}

void ComponentsCTRLFan(const tsUiProcConsts *consts, tsUiProcParams *params,
                       stFan_Flash const *fanC, stFan_Ram *fanP, bool change)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    unsigned short value = fanP->speedOrPWM;
    static unsigned char dispValue = 0;
    // TODO check with EBI
    if (fanC->type == _FanType_VDC && fanC->configs.pwmUsed)
    {
        if (change)
        {

            unsigned char compType = comps->table[comps->index][0];
            switch (compType)
            {
            case _Comp_Cooler_CondenserFan:
                value = getValueFromTable(&comps->fanTables[4]);
                dispValue = ((consts->rf->Cooler.components.condenserFan.configs.rpm / 100) * value) / 100;
                break;
            case _Comp_FF_Fan:
                if (consts->rf->Cabin[0].type == _CabinType_FF)
                    value = getValueFromTable(&comps->fanTables[0]);
                else if (consts->rf->Cabin[1].type == _CabinType_FF)
                    value = getValueFromTable(&comps->fanTables[1]);
                else if (consts->rf->Cabin[2].type == _CabinType_FF)
                    value = getValueFromTable(&comps->fanTables[2]);
                break;
            case _Comp_FRZ_Fan:
                if (consts->rf->Cabin[0].type == _CabinType_FRZ)
                    value = getValueFromTable(&comps->fanTables[0]);
                else if (consts->rf->Cabin[1].type == _CabinType_FRZ)
                    value = getValueFromTable(&comps->fanTables[1]);
                else if (consts->rf->Cabin[2].type == _CabinType_FRZ)
                    value = getValueFromTable(&comps->fanTables[2]);
                break;
            case _Comp_JKR_Fan:
                if (consts->rf->Cabin[0].type == _CabinType_JKR)
                    value = getValueFromTable(&comps->fanTables[0]);
                else if (consts->rf->Cabin[1].type == _CabinType_JKR)
                    value = getValueFromTable(&comps->fanTables[1]);
                else if (consts->rf->Cabin[2].type == _CabinType_JKR)
                    value = getValueFromTable(&comps->fanTables[2]);
                break;
            case _Comp_IceMachine_Fan:
                value = 100;

              break;
            }

            sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
        }

        if (value)
        {
            dispValue = ((fanC->configs.rpm / 100) * value) / 100;
            uiSlideData(&params->ui->slides, dispValue, params->ui->slides.slideNumber, eDisplayCabinDataTypeNumber);
        }
        else
        {
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
            dispValue = 0;
        }

        fanP->speedOrPWM = value;
        params->ui->slides.slideNumber++;
    }
    else
    {
        if (change)
        {
            if (value)
                value = 0;
            else
                value = 100;
            sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
        }

        if (value)
            uiSlideData(&params->ui->slides, eDisplaySpecialsOn, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        else
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);

        fanP->speedOrPWM = value;
        params->ui->slides.slideNumber++;
    }
}

void ComponentsCTRLFlap(const tsUiProcConsts *consts, tsUiProcParams *params,
                        stFlap_Flash const *flapC,
                        stFlap_Ram *flapP,
                        bool change)
{
    unsigned short value = flapP->value;
    switch (flapC->type)
    {
    case _FlapType_StepMotor:
        if (change)
        {
            if (value)
                value = 0;
            else
                value = 1800;
            sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
        }

        if (value)
            uiSlideData(&params->ui->slides, eDisplaySpecialsOn, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        else
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;
    }
    flapP->value = value;
    params->ui->slides.slideNumber++;
}

void ComponentsTestHeater(stHeater_Flash const *heaterC,
                          stHeater_Ram *heaterP,
                          unsigned char *testTimer)
{
    enHeaterValues value = _HeaterValue_OFF;
    switch (heaterC->type)
    {
    case _HeaterType_AC:
        if (*testTimer > _Comp_Heater_TestTime)
            *testTimer = _Comp_Heater_TestTime;

        if (*testTimer > _Comp_Heater_TestOffTime)
            value = _HeaterValue_ON;
        else
            value = _HeaterValue_OFF;
        break;
    }
    heaterP->value = value;
}

void ComponentsCTRLHeater(const tsUiProcConsts *consts, tsUiProcParams *params,
                          stHeater_Flash const *heaterC,
                          stHeater_Ram *heaterP,
                          bool change)
{
    enHeaterValues value = heaterP->value;
    switch (heaterC->type)
    {
    case _HeaterType_AC:
        if (change)
        {
            if (value)
                value = _HeaterValue_OFF;
            else
                value = _HeaterValue_ON;
            sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
        }

        if (value)
            uiSlideData(&params->ui->slides, eDisplaySpecialsOn, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        else
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;
    }
    heaterP->value = value;
    params->ui->slides.slideNumber++;
}

void ComponentsTestIonizer(stIonizer_Flash const *ionizerC,
                           stIonizer_Ram *ionizerP,
                           unsigned char *testTimer)
{
    enIonizerValues value = _IonizerValue_OFF;
    switch (ionizerC->type)
    {
    case _IonizerType_AC:
        if (*testTimer > _Comp_Ionizer_TestTime)
            *testTimer = _Comp_Ionizer_TestTime;

        if (*testTimer > _Comp_Ionizer_TestOffTime)
            value = _IonizerValue_ON;
        else
            value = _IonizerValue_OFF;
        break;
    }
    ionizerP->value = value;
}

void ComponentsCTRLIonizer(const tsUiProcConsts *consts, tsUiProcParams *params,
                           stIonizer_Flash const *ionizerC,
                           stIonizer_Ram *ionizerP,
                           bool change)
{
    enIonizerValues value = ionizerP->value;
    switch (ionizerC->type)
    {
    case _IonizerType_AC:
        if (change)
        {
            if (value)
                value = _IonizerValue_OFF;
            else
                value = _IonizerValue_ON;
            sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
        }

        if (value)
            uiSlideData(&params->ui->slides, eDisplaySpecialsOn, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        else
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;
    }
    ionizerP->value = value;
    params->ui->slides.slideNumber++;
}

void ComponentsTestVioled(stVioLed_Flash const *violedC,
                          stVioLed_Ram *violedP,
                          unsigned char *testTimer)
{
    unsigned short value = _VioLedValue_OFF;
    switch (violedC->type)
    {
    case _VioLedType_DC:
        // case _VioledType_PWM:
        if (*testTimer > _Comp_Violed_TestTime)
            *testTimer = _Comp_Violed_TestTime;

        if (*testTimer > _Comp_Violed_TestOffTime)
            value = _VioLedValue_ON;
        else
            value = _VioLedValue_OFF;
        break;
        /*
                case _VioledType_PWM:
                    if (*testTimer > _Comp_Violed_TestTime)
                        *testTimer = _Comp_Violed_TestTime;

                    if (*testTimer > _Comp_Violed_TestOffTime)
                        value = 10000;
                    else
                        value = _VioLedValue_OFF;
                break;
        */
    }
    violedP->value = value;
}

void ComponentsCTRLVioled(const tsUiProcConsts *consts, tsUiProcParams *params,
                          stVioLed_Flash const *violedC,
                          stVioLed_Ram *violedP,
                          bool change)
{
    /*enVioLedValues*/ unsigned short value = violedP->value; // ozgur
    switch (violedC->type)
    {
    case _VioLedType_DC:
        // case _VioledType_PWM:
        if (change)
        {
            if (value)
                value = _VioLedValue_OFF;
            else
                value = _VioLedValue_ON;
            sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
        }

        if (value)
            uiSlideData(&params->ui->slides, eDisplaySpecialsOn, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        else
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;
        /*
                case _VioledType_PWM:
                    if (change)
                    {
                        if (value)
                            value = _VioLedValue_OFF;
                        else
                            value = 10000;
                        sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
                    }

                    if (value)
                        uiSlideData(&params->ui->slides, eDisplaySpecialsOn, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
                    else
                        uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
                break;
        */
    }
    violedP->value = value;
    params->ui->slides.slideNumber++;
}

void ComponentsTestLight(stLight_Flash const *lightC,
                         stLight_Ram *lightP,
                         unsigned char *testTimer)
{
    unsigned short value = 0;
    switch (lightC->type)
    {
    case _LightType_AC:
    case _LightType_DC:
    case _LightType_PWM:
        if (*testTimer > _Comp_Light_TestTime)
            *testTimer = _Comp_Light_TestTime;

        if (*testTimer > _Comp_Ionizer_TestOffTime)
            value = _VioLedValue_ON; // 10000;
        else
            value = _VioLedValue_OFF; // 0;
        break;
    }

    lightP->value = value;
}

void ComponentsCTRLLight(const tsUiProcConsts *consts, tsUiProcParams *params,
                         stLight_Flash const *lightC,
                         stLight_Ram *lightP,
                         bool change)
{
    unsigned short value = lightP->value;
    switch (lightC->type)
    {
    case _LightType_AC:
    case _LightType_DC:
    case _LightType_PWM:
        if (change)
        {
            if (value)
                value = 0;
            else
                value = 10000;
            sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
        }

        if (value)
            uiSlideData(&params->ui->slides, eDisplaySpecialsOn, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        else
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;
    }

    lightP->value = value;
    params->ui->slides.slideNumber++;
}

void ComponentsCTRLHandleRGBLight(const tsUiProcConsts *consts, tsUiProcParams *params,
                         stHandleRGBLight_Flash const *lightC,
                         stHandleRGBLight_Ram *lightP,
                         bool change)
{
    unsigned short value = lightP->R_value;
    switch (lightC->type)
    {
    case _LightType_AC:
    case _LightType_DC:
    case _LightType_PWM:
        if (change)
        {
            if (value)
                value = 0;
            else
                value = 100;
            sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
        }

        if (value)
            uiSlideData(&params->ui->slides, eDisplaySpecialsOn, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        else
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;
    }

    lightP->R_value = value;
    lightP->G_value = value;
    lightP->B_value = value;
    params->ui->slides.slideNumber++;
}

void ComponentsTestRotator(stRotator_Flash const *rotatorC,
                           stRotator_Ram *rotatorP,
                           unsigned char *testTimer)
{
    enRotatorValues value = _RotatorValue_OFF;
    switch (rotatorC->type)
    {
    case _RotatorType_BDCMotor:
        if (*testTimer > _Comp_Rotator_TestTime)
            *testTimer = _Comp_Rotator_TestTime;

        if (*testTimer > _Comp_Rotator_TestCCWTime + _Comp_Rotator_TestOffTime)
            value = _RotatorValue_CW;
        else if (*testTimer > _Comp_Rotator_TestOffTime)
            value = _RotatorValue_CCW;
        else
            value = _RotatorValue_OFF;
        break;
    }
    rotatorP->value = value;
}

void ComponentsCTRLRotator(const tsUiProcConsts *consts, tsUiProcParams *params,
                           stRotator_Flash const *rotatorC,
                           stRotator_Ram *rotatorP,
                           bool change)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    enRotatorValues value = rotatorP->value;
    switch (rotatorC->type)
    {
    case _RotatorType_BDCMotor:
        switch (value)
        {
        case _RotatorValue_CW:
            if (change)
            {
                value = _RotatorValue_CCW;

                comps->secureRotatorTimer = _SecureRotatorTime;
                sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
            }
            break;

        case _RotatorValue_CCW:
            if (change)
            {
                value = _RotatorValue_OFF;
                sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
            }
            break;

        case _RotatorValue_OFF:
        default:
            if (change)
            {
                value = _RotatorValue_CW;
                comps->secureRotatorTimer = _SecureRotatorTime;
                sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
            }
            break;
        }
        if (value)
            uiSlideData(&params->ui->slides, value, params->ui->slides.slideNumber, eDisplayCabinDataTypeNumber);
        else
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;
    }
    rotatorP->value = value;
    params->ui->slides.slideNumber++;
}

void ComponentsTestMixer(stMixer_Flash const *mixerC,
                         stMixer_Ram *mixerP,
                         unsigned char *testTimer)
{
    enMixerValues value = _MixerValue_OFF;
    switch (mixerC->type)
    {
    case _MixerType_AC:
        if (*testTimer > _Comp_Mixer_TestTime)
            *testTimer = _Comp_Mixer_TestTime;

        if (*testTimer > _Comp_Mixer_TestOffTime)
            value = _MixerValue_MIX;
        else
            value = _MixerValue_OFF;
        break;
    }
    mixerP->value = value;
}

void ComponentsCTRLMixer(const tsUiProcConsts *consts, tsUiProcParams *params,
                         stMixer_Flash const *mixerC,
                         stMixer_Ram *mixerP,
                         bool change)
{
    enMixerValues value = mixerP->value;
    switch (mixerC->type)
    {
    case _MixerType_AC:
        if (change)
        {
            if (value)
                value = _MixerValue_OFF;
            else
                value = _MixerValue_MIX;
            sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
        }

        if (value)
            uiSlideData(&params->ui->slides, eDisplaySpecialsOn, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        else
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;
    }
    mixerP->value = value;
    params->ui->slides.slideNumber++;
}

void ComponentsTestPump(stPump_Flash const *pumpC,
                        stPump_Ram *pumpP,
                        unsigned char *testTimer)
{
    enPumpValues value = _PumpValue_OFF;
    switch (pumpC->type)
    {
    case _PumpType_BDCMotor:
        if (*testTimer > _Comp_Pump_TestTime)
            *testTimer = _Comp_Pump_TestTime;

        if (*testTimer > _Comp_Pump_TestVacumTime + _Comp_Pump_TestOffTime)
            value = _PumpValue_PUMP;
        else if (*testTimer > _Comp_Pump_TestOffTime)
            value = _PumpValue_VACUM;
        else
            value = _PumpValue_OFF;
        break;
    }
    pumpP->value = value;
}

void ComponentsCTRLPump(const tsUiProcConsts *consts, tsUiProcParams *params,
                        stPump_Flash const *pumpC,
                        stPump_Ram *pumpP,
                        bool change)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    enPumpValues value = pumpP->value;
    switch (pumpC->type)
    {
    case _PumpType_BDCMotor:
        switch (value)
        {
        case _PumpValue_PUMP:
            if (change)
            {
                value = _PumpValue_VACUM;
                comps->securePumpTimer = _SecurePumpTime;
                sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
            }
            break;

        case _PumpValue_VACUM:
            if (change)
            {
                value = _PumpValue_OFF;
                sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
            }
            break;

        case _PumpValue_OFF:
        default:
            if (change)
            {
                value = _PumpValue_PUMP;
                comps->securePumpTimer = _SecurePumpTime;
                sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
            }
            break;
        }

        if (value)
            uiSlideData(&params->ui->slides, value, params->ui->slides.slideNumber, eDisplayCabinDataTypeNumber);
        else
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;
    }
    pumpP->value = value;
    params->ui->slides.slideNumber++;
}

void ComponentsTestValve(stValve_Flash const *valveC,
                         stValve_Ram *valveP,
                         unsigned char *testTimer)
{
    enValveValues value = _ValveValue_CLOSE;
    switch (valveC->type)
    {
    case _ValveType_ACOut:
        if (*testTimer > _Comp_Valve_TestTime)
            *testTimer = _Comp_Valve_TestTime;

        if (*testTimer > _Comp_Valve_TestOffTime)
            value = _ValveValue_OPEN;
        else
            value = _ValveValue_CLOSE;
        break;
    }
    valveP->value = value;
}

void ComponentsCTRLValve(const tsUiProcConsts *consts, tsUiProcParams *params,
                         stValve_Flash const *valveC,
                         stValve_Ram *valveP,
                         bool change)
{
    enValveValues value = valveP->value;
    switch (valveC->type)
    {
    case _ValveType_ACOut:
        if (change)
        {
            if (value)
                value = _ValveValue_CLOSE;
            else
            {
                value = _ValveValue_OPEN;
                // secureValveTimer =  _SecureValveTime;
            }
            sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
        }

        if (value)
            uiSlideData(&params->ui->slides, eDisplaySpecialsOn, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        else
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;
    }
    valveP->value = value;
    params->ui->slides.slideNumber++;
}

void ComponentsTestSelector(stSelector_Flash const *selectorC,
                            stSelector_Ram *selectorP,
                            unsigned char *testTimer)
{
    enSelectorValues value = _SelectorValue_OFF;
    switch (selectorC->type)
    {
    case _SelectorType_BDCMotor:
        if (*testTimer > _Comp_Selector_TestTime)
            *testTimer = _Comp_Selector_TestTime;

        if (*testTimer > _Comp_Selector_TestSelectBTime + _Comp_Selector_TestOffTime)
            value = _SelectorValue_SELECTING_A;
        else if (*testTimer > _Comp_Selector_TestOffTime)
            value = _SelectorValue_SELECTING_B;
        else
            value = _SelectorValue_OFF;
        break;
    case _SelectorType_ACMotor:
      if (*testTimer > _Comp_Pusher_TestTime)
        *testTimer = _Comp_Pusher_TestTime;
      
      if (*testTimer > _Comp_Pusher_TestOffTime)
        value = _SelectorValue_SELECTING_A;
      else
        value = _SelectorValue_OFF;
      break;
    }
    selectorP->value = value;
}

void ComponentsCTRLSelector(const tsUiProcConsts *consts, tsUiProcParams *params,
                            stSelector_Flash const *selectorC,
                            stSelector_Ram *selectorP,
                            bool change)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    enSelectorValues value = selectorP->value;
    switch (selectorC->type)
    {
    case _SelectorType_BDCMotor:
        switch (value)
        {
        case _SelectorValue_SELECTING_A:
            if (change)
            {
                value = _SelectorValue_SELECTING_B;
                comps->secureSelectorTimer = _SecureSelectorTime;
                sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
            }
            break;

        case _SelectorValue_SELECTING_B:
            if (change)
            {
                value = _SelectorValue_OFF;
                sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
            }
            break;

        case _SelectorValue_OFF:
        default:
            if (change)
            {
                value = _SelectorValue_SELECTING_A;
                comps->secureSelectorTimer = _SecureSelectorTime;
                sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
            }
            break;
        }
        if (value)
          uiSlideData(&params->ui->slides, value, params->ui->slides.slideNumber, eDisplayCabinDataTypeNumber);
        else
          uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;
    case _SelectorType_ACMotor:
      if (change)
      {
        if (value)
          value = _SelectorValue_OFF;
        else
          value = _SelectorValue_SELECTING_A;
        comps->secureSelectorTimer = _SecureSelectorTime*2;
        sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
      }
      
      if (value)
        uiSlideData(&params->ui->slides, eDisplaySpecialsOn, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
      else
        uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
      break;


    }
    selectorP->value = value;
    params->ui->slides.slideNumber++;
}

void ComponentsTestShutter(stShutter_Flash const *shutterC,
                           stShutter_Ram *shutterP,
                           unsigned char *testTimer)
{
    enShutterValues value = _ShutterValue_OFF;
    switch (shutterC->type)
    {
    case _ShutterType_BDCMotor:
        if (*testTimer > _Comp_Selector_TestTime)
            *testTimer = _Comp_Selector_TestTime;

        if (*testTimer > _Comp_Shutter_TestClosingTime + _Comp_Shutter_TestOffTime)
            value = _ShutterValue_OPENING;
        else if (*testTimer > _Comp_Shutter_TestOffTime)
            value = _ShutterValue_CLOSING;
        else
            value = _ShutterValue_OFF;
        break;
    case _ShutterType_ACMotor:
        if (*testTimer > _Comp_Pusher_TestTime)
            *testTimer = _Comp_Pusher_TestTime;

        if (*testTimer > _Comp_Pusher_TestOffTime)
            value = _ShutterValue_OPENING;
        else
            value = _ShutterValue_OFF;
      break;
    }
    shutterP->value = value;
}

void ComponentsCTRLShutter(const tsUiProcConsts *consts, tsUiProcParams *params,
                           stShutter_Flash const *shutterC,
                           stShutter_Ram *shutterP,
                           bool change)
{
    tsUiSmCompsData *comps = &params->ui->states.service.comps;

    enShutterValues value = shutterP->value;
    switch (shutterC->type)
    {
    case _ShutterType_BDCMotor:
        switch (value)
        {
        case _ShutterValue_OPENING:
            if (change)
            {
                value = _ShutterValue_CLOSING;
                comps->secureShutterTimer = _SecureShutterTime;
                sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
            }
            break;

        case _ShutterValue_CLOSING:
            if (change)
            {
                value = _ShutterValue_OFF;
                sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
            }
            break;

        case _ShutterValue_OFF:
        default:
            if (change)
            {
                value = _ShutterValue_OPENING;
                comps->secureShutterTimer = _SecureShutterTime;
                sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
            }
            break;
        }
        if (value)
            uiSlideData(&params->ui->slides, value, params->ui->slides.slideNumber, eDisplayCabinDataTypeNumber);
        else
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;
        
      case _ShutterType_ACMotor:
        if (change)
        {
          if (value)
            value = _ShutterValue_OFF;
          else
            value = _ShutterValue_OPENING;
          comps->secureShutterTimer = _SecureShutterTime*2;
          sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
        }
        
        if (value)
          uiSlideData(&params->ui->slides, eDisplaySpecialsOn, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        else
          uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;
    }
    shutterP->value = value;
    params->ui->slides.slideNumber++;
}

void ComponentsTestPusher(stPusher_Flash const *pusherC,
                          stPusher_Ram *pusherP,
                          unsigned char *testTimer)
{
    enPusherValues value = _PusherValue_OFF;
    switch (pusherC->type)
    {
    case _PusherType_ACOut:
        if (*testTimer > _Comp_Pusher_TestTime)
            *testTimer = _Comp_Pusher_TestTime;

        if (*testTimer > _Comp_Pusher_TestOffTime)
            value = _PusherValue_PUSH;
        else
            value = _PusherValue_OFF;
        break;
    }
    pusherP->value = value;
}

void ComponentsCTRLPusher(const tsUiProcConsts *consts, tsUiProcParams *params,
                          stPusher_Flash const *pusherC,
                          stPusher_Ram *pusherP,
                          bool change)
{
    enPusherValues value = pusherP->value;
    switch (pusherC->type)
    {
    case _PusherType_ACOut:
        if (change)
        {
            if (value)
                value = _PusherValue_OFF;
            else
                value = _PusherValue_PUSH;
            sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
        }

        if (value)
            uiSlideData(&params->ui->slides, eDisplaySpecialsOn, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        else
            uiSlideData(&params->ui->slides, eDisplaySpecialsOff, params->ui->slides.slideNumber, eDisplayCabinDataTypeSpecial);
        break;
    }
    pusherP->value = value;
    params->ui->slides.slideNumber++;
}
#endif //SERVICEMODEavailable
