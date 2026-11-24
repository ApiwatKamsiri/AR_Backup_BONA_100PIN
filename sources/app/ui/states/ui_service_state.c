#include "ui_service_state.h"
#include "service_mode_procedures/ui_sm_components.h"
#include "service_mode_procedures/ui_sm_parameters.h"
#include "service_mode_procedures/ui_sm_sensors.h"
#include "refrigerator/refrigerator.h"

#include "../components/ui_display.h"

#include "../procedures/ui_slides.h"

#include "../command_events/cmd_check.h"
#include "../command_events/ce_service.h"
#include "../command_events/ce_buzzer.h"

#include "app/refrigerator/procedures/rf_errors.h"
#include "dev/display_spi.h"

#ifdef HOSHI_H30
#include "app/ice_machine/ice_machine_procedures.h"
#include "app/water_source/water_source_procedures.h"
#endif  //HOSHI H30 and ALPS H10/30

#ifdef SERVICEMODEavailable

#define _UI_ServiceAutoExitTime20Ms (10u * 60u * 50u)

#ifdef HOSHI_H30
#define _UI_ServiceTestTimeout20Ms (_SEC_T20ms(50))
#define _UI_ServiceBlinkToggle20Ms 25
#define _UI_ServiceWarningBeepInterval20Ms 75
#define _UI_ServiceWarningBeepCount 5

typedef enum
{
    _UI_ServiceIceDumpPhase_ToStartBeforeDump = 0,
    _UI_ServiceIceDumpPhase_ToDump,
    _UI_ServiceIceDumpPhase_ToStartAfterDump,
} enUI_ServiceIceDumpPhase;

typedef enum
{
    _UI_ServicePumpPhase_Pump = 0,
    _UI_ServicePumpPhase_Vacum,
} enUI_ServicePumpPhase;

typedef struct
{
    bool running;
    bool ledBlinkState;
    unsigned short timeout20Ms;
    unsigned char blinkTimer20Ms;
    enUI_ServiceIceDumpPhase phase;
} tsUI_ServiceIceDumpTest;

typedef struct
{
    bool running;
    bool ledBlinkState;
    unsigned short timeout20Ms;
    unsigned char blinkTimer20Ms;
    enUI_ServicePumpPhase phase;
} tsUI_ServicePumpTest;

static tsUI_ServiceIceDumpTest uiServiceIceDumpTest;
static tsUI_ServicePumpTest uiServicePumpTest;
static bool uiServiceIceDumpCompleted;
static bool uiServicePumpCompleted;
static unsigned char uiServiceWarningBeepRemaining;
static unsigned char uiServiceWarningBeepTimer20Ms;

static void _uiServiceStartWarningBeeps(void)
{
    uiServiceWarningBeepRemaining = _UI_ServiceWarningBeepCount;
    uiServiceWarningBeepTimer20Ms = 0;
}

static void _uiServiceSetIceMachineWaterDesired(tsUiProcParams *params, enWaterSourceValues desired)
{
    params->rf->IceMachine.values.waterDesired = desired;
    params->rf->WaterSource.values.iceMachineDesired = desired;
    WaterSupplyType_INTERNAL_IceMachine_Dispenser(&params->rf->WaterSource);
}

static void _uiServiceStopIceDumpTest(tsUiProcParams *params)
{
    stIceMachine_Ram *iceMachineP = &params->rf->IceMachine;

    RotateStop(iceMachineP);
    _uiServiceSetIceMachineWaterDesired(params, _WaterSourceValue_OFF);
    uiServiceIceDumpTest.running = false;
}

static void _uiServiceStopPumpTest(tsUiProcParams *params)
{
    stIceMachine_Ram *iceMachineP = &params->rf->IceMachine;

    RotateStop(iceMachineP);
    _uiServiceSetIceMachineWaterDesired(params, _WaterSourceValue_OFF);
    uiServicePumpTest.running = false;
}

static void _uiServiceStopAllTests(tsUiProcParams *params)
{
    _uiServiceStopIceDumpTest(params);
    _uiServiceStopPumpTest(params);
}

static void _uiServiceStartIceDumpTest(tsUiProcParams *params)
{
    _uiServiceStopAllTests(params);

    sysBuzzerCmd(eSysBuzzerPlayStartMelody);

    uiServiceIceDumpCompleted = false;
    uiServiceIceDumpTest.running = true;
    uiServiceIceDumpTest.phase = _UI_ServiceIceDumpPhase_ToStartBeforeDump;
    uiServiceIceDumpTest.timeout20Ms = _UI_ServiceTestTimeout20Ms;
    uiServiceIceDumpTest.blinkTimer20Ms = 0;
    uiServiceIceDumpTest.ledBlinkState = false;
}

static void _uiServiceStartPumpTest(tsUiProcParams *params)
{
    stIceMachine_Ram *iceMachineP = &params->rf->IceMachine;

    _uiServiceStopAllTests(params);

    sysBuzzerCmd(eSysBuzzerPlayStartMelody);

    uiServicePumpCompleted = false;
    uiServicePumpTest.running = true;
    uiServicePumpTest.phase = _UI_ServicePumpPhase_Pump;
    uiServicePumpTest.timeout20Ms = _UI_ServiceTestTimeout20Ms;
    uiServicePumpTest.blinkTimer20Ms = 0;
    uiServicePumpTest.ledBlinkState = false;

    RotateStop(iceMachineP);
    iceMachineP->values.waterPumpTimer = (iceMachineP->sets.waterPumpTime * 5);
    _uiServiceSetIceMachineWaterDesired(params, _WaterSourceValue_PUMP);
}
#endif  //HOSHI H30 and ALPS H10/30

static unsigned short uiServiceAutoExitTimer20Ms;

void uiServiceStateInit(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;

    uiP->states.service.flags.alls = 0;
    uiP->states.service.timer = 0;
    uiP->states.service.state = _UI_ServiceState_BlinkLed;
    uiServiceAutoExitTimer20Ms = _UI_ServiceAutoExitTime20Ms;
#ifdef HOSHI_H30
    _uiServiceStopAllTests(params);
    uiServiceIceDumpCompleted = false;
    uiServicePumpCompleted = false;
    uiServiceWarningBeepRemaining = 0;
    uiServiceWarningBeepTimer20Ms = 0;
#endif  //HOSHI H30 and ALPS H10/30
    uiSmCompsInit(consts, params);
    uiSmCompsCheckAll(consts, params);
    uiSmCompsCloseAll(consts, params);

    uiSmCompsInitFanTables(consts, params);
    uiSmCompsInitVCCFreqTable(consts, params);
}

void uiServiceState20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;

    if (uiServiceAutoExitTimer20Ms)
    {
        uiServiceAutoExitTimer20Ms--;

        if (!uiServiceAutoExitTimer20Ms)
        {
#ifdef HOSHI_H30
            _uiServiceStopAllTests(params);
            uiServiceIceDumpCompleted = false;
            uiServicePumpCompleted = false;
            uiP->dspPacket.funcs.ledIceOn = false;
            uiP->dspPacket.funcs.ledEcoExtra = false;
#endif  //HOSHI H30 and ALPS H10/30
            ceServiceExit(uiP, true);
        }
    }

    if (uiP->states.service.timer)
        uiP->states.service.timer--;

#ifdef HOSHI_H30
    if (uiServiceWarningBeepRemaining)
    {
        if (uiServiceWarningBeepTimer20Ms)
        {
            uiServiceWarningBeepTimer20Ms--;
        }
        else
        {
            sysBuzzerCmd(eSysBuzzerPlayWarningMelody);
            uiServiceWarningBeepRemaining--;
            uiServiceWarningBeepTimer20Ms = _UI_ServiceWarningBeepInterval20Ms;
        }
    }

    if (uiServiceIceDumpTest.running)
    {
        if (uiServiceIceDumpTest.timeout20Ms)
            uiServiceIceDumpTest.timeout20Ms--;

        uiServiceIceDumpTest.blinkTimer20Ms++;
        if (uiServiceIceDumpTest.blinkTimer20Ms >= _UI_ServiceBlinkToggle20Ms)
        {
            uiServiceIceDumpTest.blinkTimer20Ms = 0;
            uiServiceIceDumpTest.ledBlinkState = !uiServiceIceDumpTest.ledBlinkState;
        }
    }

    if (uiServicePumpTest.running)
    {
        stIceMachine_Ram *iceMachineP = &params->rf->IceMachine;

        if (uiServicePumpTest.timeout20Ms)
            uiServicePumpTest.timeout20Ms--;

        if (iceMachineP->values.waterPumpTimer)
            iceMachineP->values.waterPumpTimer--;

        if (iceMachineP->values.waterVacumTimer)
            iceMachineP->values.waterVacumTimer--;

        uiServicePumpTest.blinkTimer20Ms++;
        if (uiServicePumpTest.blinkTimer20Ms >= _UI_ServiceBlinkToggle20Ms)
        {
            uiServicePumpTest.blinkTimer20Ms = 0;
            uiServicePumpTest.ledBlinkState = !uiServicePumpTest.ledBlinkState;
        }
    }
#endif  //HOSHI H30 and ALPS H10/30

    uiSmComps20MsTask(consts, params);
}

void uiServiceStateTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;
    
    //tsUiSmCompsData *comps = &params->ui->states.service.comps;
    //unsigned char compIndex = comps->table[comps->index][1];

    uiP->dspPacket.funcs.displayOff = false;
    rfErrorsCheckAll(consts->rf, params->rf);

    switch (uiP->states.service.state)
    {
    case _UI_ServiceState_DispVersionRevision:
        if (!uiP->states.service.timer)
            uiP->states.service.timer = 100;

        /*
        if (display_F.SubType == _DisplayType_DUMMY_v0)
        {
            for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
            {
                if (i == consts->rf->Parameters.displayData)
                {
                    if (uiP->states.service.timer > 75)
                        uiDspCabinData(consts, params, uiP->dspPacket.displayInfo.swVer, i, eDisplayCabinDataTypeNumber);
                    else if (uiP->states.service.timer > 50)
                        uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
                    else if (uiP->states.service.timer > 25)
                        uiDspCabinData(consts, params, uiP->dspPacket.displayInfo.swRev, i, eDisplayCabinDataTypeNumber);
                    else
                        uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
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
            if (uiP->states.service.timer > 25)
            {
                for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
                {
                    if (i == consts->rf->Parameters.displayData)
                        uiDspCabinData(consts, params, uiP->dspPacket.displayInfo.swVer, i, eDisplayCabinDataTypeNumber);
                    else if (i == consts->rf->Parameters.displayValue)
                        uiDspCabinData(consts, params, uiP->dspPacket.displayInfo.swRev, i, eDisplayCabinDataTypeNumber);
                    else if (i == consts->rf->Parameters.displayUnit)
                        uiDspCabinData(consts, params, eDisplaySpecialsCDegree, i, eDisplayCabinDataTypeSpecial);
                    else
                        uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
                }
            }
            else
            {
                for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
                    uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
            }
        }

        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);
        uiDspFunctionsOff(consts, params);
        break;

    case _UI_ServiceState_VersionRevision:
        if (!uiP->states.service.timer)
            uiP->states.service.timer = 100;

        /*
        if (display_F.SubType == _DisplayType_DUMMY_v0)
        {
            for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
            {
                if (i == consts->rf->Parameters.displayData)
                {
                    if (uiP->states.service.timer > 75)
                        uiDspCabinData(consts, params, consts->rf->Stock.Version, i, eDisplayCabinDataTypeNumber);
                    else if (uiP->states.service.timer > 50)
                        uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
                    else if (uiP->states.service.timer > 25)
                        uiDspCabinData(consts, params, consts->rf->Stock.Revision, i, eDisplayCabinDataTypeNumber);
                    else
                        uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
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
            if (uiP->states.service.timer > 25)
            {
                for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
                {
                    if (i == consts->rf->Parameters.displayData)
                        uiDspCabinData(consts, params, consts->rf->Stock.Version, i, eDisplayCabinDataTypeNumber);
                    else if (i == consts->rf->Parameters.displayValue)
                        uiDspCabinData(consts, params, consts->rf->Stock.Revision, i, eDisplayCabinDataTypeNumber);
                    else if (i == consts->rf->Parameters.displayUnit)
                        uiDspCabinData(consts, params, eDisplaySpecialsCDegree, i, eDisplayCabinDataTypeSpecial);
                    else
                        uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
                }
            }
            else
            {
                for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
                    uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
            }
        }

        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);
        uiDspFunctionsOff(consts, params);
        break;

    case _UI_ServiceState_ShowNTC:
#ifdef HOSHI_H30
        uiDspAllOff(consts, params);
        uiP->dspPacket.funcs.ledIceOn = uiServiceIceDumpCompleted;
        uiP->dspPacket.funcs.ledEcoExtra = uiServicePumpCompleted;
#else
        uiSmSensCheckAll(consts, params);
        uiSmSensControlSelected(consts, params);

        if (!uiP->states.service.timer)
        {
            uiP->slides.slideIndex++;
            if (uiP->slides.slideIndex >= uiP->slides.slideNumber)
            {
                uiP->slides.slideIndex = 0;
                ceServiceIncreaseSensorIndex(uiP, true);
                uiP->states.service.timer = 75;
            }
            else
            {
                uiP->states.service.timer = 25;
            }
        }

        if (uiP->states.service.flags.allOff)
            uiSmCompsCloseAll(consts, params);

        if (uiP->states.service.timer > 50)
        {
            for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
                uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
        }
        else
        {
            for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
            {
                if (i == consts->rf->Parameters.displayData)
                    uiDspCabinData(consts, params, uiP->slides.slide[uiP->slides.slideIndex].data, i, uiP->slides.slide[uiP->slides.slideIndex].dataType);
                else if (i == consts->rf->Parameters.displayValue)
                    uiDspCabinData(consts, params, uiP->slides.slide[uiP->slides.slideNumber - 1].data, i, uiP->slides.slide[uiP->slides.slideNumber - 1].dataType);
                else if (i == consts->rf->Parameters.displayUnit)
                    uiDspCabinData(consts, params, eDisplaySpecialsCDegree, i, eDisplayCabinDataTypeSpecial);
                else
                    uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
            }
        }
        //uiP->dspPacket.funcs.ledAlert = (params->rf->Values.errors.sensorErrorsNumber) ? true : false;  //APW : Disable for BONA

        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);
        uiDspDoorsAtQuick(consts, params);
#endif  //HOSHI H30 and ALPS H10/30
        break;
    case _UI_ServiceState_AllOnAllOff:
#ifdef HOSHI_H30
        {
            stIceMachine_Ram *iceMachineP = &params->rf->IceMachine;

            uiDspAllOff(consts, params);
            uiP->dspPacket.funcs.ledIceOn = uiServiceIceDumpTest.ledBlinkState;
            uiP->dspPacket.funcs.ledEcoExtra = uiServicePumpCompleted;

            if (!uiServiceIceDumpTest.running)
            {
                uiP->states.service.state = _UI_ServiceState_ShowNTC;
                break;
            }

            if (!uiServiceIceDumpTest.timeout20Ms)
            {
                _uiServiceStopIceDumpTest(params);
                uiServiceIceDumpCompleted = false;
                _uiServiceStartWarningBeeps();
                uiP->dspPacket.funcs.ledIceOn = false;
                uiP->states.service.state = _UI_ServiceState_ShowNTC;
                break;
            }

            switch (uiServiceIceDumpTest.phase)
            {
            case _UI_ServiceIceDumpPhase_ToStartBeforeDump:
                RotateToSTART(iceMachineP);
                _uiServiceSetIceMachineWaterDesired(params, _WaterSourceValue_OFF);

                if (iceMachineP->values.step == _IMch_Step_0)
                    uiServiceIceDumpTest.phase = _UI_ServiceIceDumpPhase_ToDump;
                break;

            case _UI_ServiceIceDumpPhase_ToDump:
                RotateToEND(iceMachineP);
                _uiServiceSetIceMachineWaterDesired(params, _WaterSourceValue_OFF);

                if (iceMachineP->values.step == _IMch_Step_6)
                    uiServiceIceDumpTest.phase = _UI_ServiceIceDumpPhase_ToStartAfterDump;
                break;

            case _UI_ServiceIceDumpPhase_ToStartAfterDump:
            default:
                RotateToSTART(iceMachineP);
                _uiServiceSetIceMachineWaterDesired(params, _WaterSourceValue_OFF);

                if (iceMachineP->values.step == _IMch_Step_0)
                {
                    _uiServiceStopIceDumpTest(params);
                    uiServiceIceDumpCompleted = true;
                    sysBuzzerCmd(eSysBuzzerPlayEndMelody);
                    uiP->dspPacket.funcs.ledIceOn = true;
                    uiP->states.service.state = _UI_ServiceState_ShowNTC;
                }
                break;
            }
        }
#else
        if (uiP->states.service.flags.specialsChange)
        {
            uiP->states.service.flags.specialsOnOff = !uiP->states.service.flags.specialsOnOff;
            uiSmCompsControlSpecials(consts, params,uiP->states.service.flags.specialsOnOff);

            if (uiP->states.service.flags.specialsOnOff)
                uiSmCompsSetFansCurrentIndexToLastIndex(consts, params);
            else
                uiSmCompsSetFansCurrentIndexToInit(consts, params);
        }

        // Prepare uiP->slides.slide
        uiSlideData(&uiP->slides, eDisplaySpecialsComponents, 0, eDisplayCabinDataTypeSpecial);
        if (uiP->states.service.flags.specialsOnOff)
        {
            uiSlideData(&uiP->slides, eDisplaySpecialsOn, 1, eDisplayCabinDataTypeSpecial);
        }
        else
        {
            uiSlideData(&uiP->slides, eDisplaySpecialsOff, 1, eDisplayCabinDataTypeSpecial);
        }
        uiP->slides.slideNumber = 2;

        // Slide animation
        if (!uiP->states.service.timer)
        {
            uiP->slides.slideIndex++;
            if (uiP->slides.slideIndex >= uiP->slides.slideNumber)
            {
                uiP->slides.slideIndex = 0;
                uiP->states.service.timer = 75;
            }
            else
            {
                uiP->states.service.timer = 25;
            }
        }

        for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
        {
            if (i == consts->rf->Parameters.displayData)
                uiDspCabinData(consts, params, uiP->slides.slide[uiP->slides.slideIndex].data, i, uiP->slides.slide[uiP->slides.slideIndex].dataType);
            else if (i == consts->rf->Parameters.displayValue)
                uiDspCabinData(consts, params, uiP->slides.slide[uiP->slides.slideNumber - 1].data, i, uiP->slides.slide[uiP->slides.slideNumber - 1].dataType);
            else
                uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
        }
        //uiP->dspPacket.funcs.ledAlert = (params->rf->Values.errors.fanErrorsNumber | params->rf->Values.errors.sensorErrorsNumber) ? true : false;    //APW : Disable for BONA
        ;

        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);
        uiDspDoorsAtQuick(consts, params);
#endif  //HOSHI H30 and ALPS H10/30
        break;

    case _UI_ServiceState_ShowComponent:
#ifdef HOSHI_H30
        {
            stIceMachine_Ram *iceMachineP = &params->rf->IceMachine;

            uiDspAllOff(consts, params);
            uiP->dspPacket.funcs.ledIceOn = uiServiceIceDumpCompleted;
            uiP->dspPacket.funcs.ledEcoExtra = uiServicePumpTest.ledBlinkState;

            if (!uiServicePumpTest.running)
            {
                uiP->states.service.state = _UI_ServiceState_ShowNTC;
                break;
            }

            RotateStop(iceMachineP);

            if (!uiServicePumpTest.timeout20Ms)
            {
                _uiServiceStopPumpTest(params);
                uiServicePumpCompleted = false;
                _uiServiceStartWarningBeeps();
                uiP->dspPacket.funcs.ledEcoExtra = false;
                uiP->states.service.state = _UI_ServiceState_ShowNTC;
                break;
            }

            switch (uiServicePumpTest.phase)
            {
            case _UI_ServicePumpPhase_Pump:
                _uiServiceSetIceMachineWaterDesired(params, _WaterSourceValue_PUMP);

                if (!iceMachineP->values.waterPumpTimer)
                {
                    iceMachineP->values.waterVacumTimer = _WaterVacumTime;
                    _uiServiceSetIceMachineWaterDesired(params, _WaterSourceValue_VACUM);
                    uiServicePumpTest.phase = _UI_ServicePumpPhase_Vacum;
                }
                break;

            case _UI_ServicePumpPhase_Vacum:
            default:
                _uiServiceSetIceMachineWaterDesired(params, _WaterSourceValue_VACUM);

                if (!iceMachineP->values.waterVacumTimer)
                {
                    _uiServiceStopPumpTest(params);
                    uiServicePumpCompleted = true;
                    sysBuzzerCmd(eSysBuzzerPlayEndMelody);
                    uiP->dspPacket.funcs.ledEcoExtra = true;
                    uiP->states.service.state = _UI_ServiceState_ShowNTC;
                }
                break;
            }
        }
#else
        uiSmCompsCheckAll(consts, params);
        uiSmCompsControlSelected(consts, params, uiP->states.service.flags.selectedChange);

        if (uiP->states.service.flags.specialsChange)
        {
            uiP->states.service.flags.specialsOnOff = !uiP->states.service.flags.specialsOnOff;
            uiSmCompsControlSpecials(consts, params, uiP->states.service.flags.specialsOnOff);
        }

        if (uiP->states.service.flags.allOff)
            uiSmCompsCloseAll(consts, params);

        uiSmCompsSecureAll(consts, params);

        if (!uiP->states.service.timer)
        {
            uiP->slides.slideIndex++;
            if (uiP->slides.slideIndex >= uiP->slides.slideNumber)
            {
                uiP->slides.slideIndex = 0;
                uiP->states.service.timer = 75;
            }
            else
            {
                uiP->states.service.timer = 25;
            }
        }

        for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
        {
            if (i == consts->rf->Parameters.displayData)
                uiDspCabinData(consts, params, uiP->slides.slide[uiP->slides.slideIndex].data, i, uiP->slides.slide[uiP->slides.slideIndex].dataType);
            else if (i == consts->rf->Parameters.displayValue)
                uiDspCabinData(consts, params, uiP->slides.slide[uiP->slides.slideNumber - 1].data, i, uiP->slides.slide[uiP->slides.slideNumber - 1].dataType);
            else
                uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
        }
        //uiP->dspPacket.funcs.ledAlert = (params->rf->Values.errors.fanErrorsNumber) ? true : false;   //APW : Disable for BONA

        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);
        uiDspDoorsAtQuick(consts, params);
#endif  //HOSHI H30 and ALPS H10/30
        break;

    case _UI_ServiceState_ShowParameter:
        uiSmParamsCheckAll(consts, params);
        uiSmParamsControlSelected(consts, params, uiP->states.service.flags.selectedChange);

        if (!uiP->states.service.timer)
        {
            uiP->slides.slideIndex++;
            if (uiP->slides.slideIndex >= uiP->slides.slideNumber)
            {
                uiP->slides.slideIndex = 0;
                uiP->states.service.timer = 75;
            }
            else
            {
                uiP->states.service.timer = 25;
            }
        }

        for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
        {
            if (i == consts->rf->Parameters.displayData)
                uiDspCabinData(consts, params, uiP->slides.slide[uiP->slides.slideIndex].data, i, uiP->slides.slide[uiP->slides.slideIndex].dataType);
            else if (i == consts->rf->Parameters.displayValue)
                uiDspCabinData(consts, params, uiP->slides.slide[uiP->slides.slideNumber - 1].data, i, uiP->slides.slide[uiP->slides.slideNumber - 1].dataType);
            else
                uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
        }

        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);
        uiDspDoorsAtQuick(consts, params);
        break;

    case _UI_ServiceState_BlinkLed:
    default:
        if (!uiP->states.service.timer)
            uiP->states.service.timer = 100;

        // Wait for 1.5 second (is it elapsed?)
        if (uiP->states.service.timer > 25)
            uiDspAllOn(consts, params);
        else
            uiDspAllOff(consts, params);

        break;
    }

    uiP->states.service.flags.specialsChange = 0;
    uiP->states.service.flags.selectedChange = 0;
    uiP->states.service.flags.allOff = 0;
}

void uiServiceStateKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;
#ifdef HOSHI_H30
    bool passwordEvent = isEvent(eDisplayEventPassword);
    bool serviceModeEvent = isEvent(eDisplayEventServiceMode);
    bool testsRunning = uiServiceIceDumpTest.running || uiServicePumpTest.running;
#endif  //HOSHI H30 and ALPS H10/30

    if (uiP->dspValues.receivedEvent == eDisplayEventKeyLock)
        uiP->dspValues.receivedEvent = eDisplayEventStateMenu;

    if (!params->rf->Values.keyLock)
#ifdef HOSHI_H30
        ceServiceExit(uiP, passwordEvent);
#else
        ceServiceExit(uiP, isEvent(eDisplayEventPassword));
#endif  //HOSHI H30 and ALPS H10/30

#ifdef HOSHI_H30
    if (serviceModeEvent && !testsRunning)
    {
        uiServiceWarningBeepRemaining = 0;
        uiServiceWarningBeepTimer20Ms = 0;
        ceServiceExit(uiP, true);
    }

    if (passwordEvent)
    {
        _uiServiceStopAllTests(params);
        uiServiceIceDumpCompleted = false;
        uiServicePumpCompleted = false;
        uiServiceWarningBeepRemaining = 0;
        uiServiceWarningBeepTimer20Ms = 0;
        uiP->dspPacket.funcs.ledIceOn = false;
        uiP->dspPacket.funcs.ledEcoExtra = false;
    }
#endif  //HOSHI H30 and ALPS H10/30

    switch (uiP->states.service.state)
    {
    case _UI_ServiceState_DispVersionRevision:
        for (unsigned char i = eDisplayEventC0SetUp; i < eDisplayEventIceCreamMchOnOff; i++)
        {
            ceServiceSetState(uiP, _UI_ServiceState_VersionRevision, isEvent((teDisplayEvent)i));
        }
        ceServiceSetState(uiP, _UI_ServiceState_VersionRevision, isEvent(eDisplayEventSelectCabin));
        ceServiceSetState(uiP, _UI_ServiceState_VersionRevision, isEvent(eDisplayEventSetCabin));
        break;

    case _UI_ServiceState_VersionRevision:
        for (unsigned char i = eDisplayEventC0SetUp; i < eDisplayEventIceCreamMchOnOff; i++)
        {
            ceServiceSetState(uiP, _UI_ServiceState_ShowNTC, isEvent((teDisplayEvent)i));
        }
        ceServiceSetState(uiP, _UI_ServiceState_ShowNTC, isEvent(eDisplayEventSelectCabin));
        ceServiceSetState(uiP, _UI_ServiceState_ShowNTC, isEvent(eDisplayEventSetCabin));
        break;

    case _UI_ServiceState_ShowNTC:
#ifdef HOSHI_H30
        if (isEvent(eDisplayEventIceOn))
        {
            _uiServiceStartIceDumpTest(params);
            ceServiceSetState(uiP, _UI_ServiceState_AllOnAllOff, true);
        }

        if (isEvent(eDisplayEventEcoWhirlpool) || isEvent(eDisplayEventEcoExtra))
        {
            _uiServiceStartPumpTest(params);
            ceServiceSetState(uiP, _UI_ServiceState_ShowComponent, true);
        }
#else
        ceServiceIncreaseSensorIndex(uiP, isEvent((teDisplayEvent)(eDisplayEventC0SetDown + consts->rf->Parameters.displayData)));
        ceServiceIncreaseSensorIndex(uiP, isEvent(eDisplayEventSetCabin));
        ceServiceDecreaseSensorIndex(uiP, isEvent((teDisplayEvent)(eDisplayEventC0QCool + consts->rf->Parameters.displayData)));
        ceServiceAllOff(uiP, isEvent(eDisplayEventVacation));

        ceServiceSetState(uiP, _UI_ServiceState_AllOnAllOff, isEvent(eDisplayEventStateMenu));
#endif  //HOSHI H30 and ALPS H10/30
        break;

    case _UI_ServiceState_AllOnAllOff:
#ifdef HOSHI_H30
        if (isEvent(eDisplayEventIceOn))
        {
            _uiServiceStartIceDumpTest(params);
            ceServiceSetState(uiP, _UI_ServiceState_AllOnAllOff, true);
        }
#else
        ceServiceSpecialsChange(uiP, isEvent(eDisplayEventSelectCabin));
        ceServiceSpecialsChange(uiP, isEvent((teDisplayEvent)(eDisplayEventC0SetDown + consts->rf->Parameters.displayValue)));

        ceServiceSetState(uiP, _UI_ServiceState_ShowComponent, isEvent(eDisplayEventStateMenu));
#endif  //HOSHI H30 and ALPS H10/30
        break;

    case _UI_ServiceState_ShowComponent:
#ifdef HOSHI_H30
        if (isEvent(eDisplayEventEcoWhirlpool) || isEvent(eDisplayEventEcoExtra))
        {
            _uiServiceStartPumpTest(params);
            ceServiceSetState(uiP, _UI_ServiceState_ShowComponent, true);
        }
#else
        ceServiceIncreaseComponentIndex(uiP, isEvent((teDisplayEvent)(eDisplayEventC0SetDown + consts->rf->Parameters.displayData)));
        ceServiceIncreaseComponentIndex(uiP, isEvent(eDisplayEventSetCabin));
        ceServiceDecreaseComponentIndex(uiP, isEvent((teDisplayEvent)(eDisplayEventC0QCool + consts->rf->Parameters.displayData)));

        ceServiceSelectedChange(uiP, isEvent(eDisplayEventSelectCabin));
        ceServiceSelectedChange(uiP, isEvent((teDisplayEvent)(eDisplayEventC0SetDown + consts->rf->Parameters.displayValue)));

        ceServiceSpecialsChange(uiP, isEvent(eDisplayEventComponentsOn));
        ceServiceAllOff(uiP, isEvent(eDisplayEventVacation));

        ceServiceSetState(uiP, _UI_ServiceState_ShowParameter, isEvent(eDisplayEventStateMenu));
#endif  //HOSHI H30 and ALPS H10/30
        break;

    case _UI_ServiceState_ShowParameter:
        ceServiceIncreaseParameterIndex(uiP, isEvent((teDisplayEvent)(eDisplayEventC0SetDown + consts->rf->Parameters.displayData)));
        ceServiceIncreaseParameterIndex(uiP, isEvent(eDisplayEventSetCabin));
        ceServiceDecreaseParameterIndex(uiP, isEvent((teDisplayEvent)(eDisplayEventC0QCool + consts->rf->Parameters.displayData)));

        ceServiceSelectedChange(uiP, isEvent(eDisplayEventSelectCabin));
        ceServiceSelectedChange(uiP, isEvent((teDisplayEvent)(eDisplayEventC0SetDown + consts->rf->Parameters.displayValue)));

        ceServiceSetState(uiP, _UI_ServiceState_ShowNTC, isEvent(eDisplayEventStateMenu));
        break;

    case _UI_ServiceState_BlinkLed:
    default:
#ifdef HOSHI_H30
        uiServiceIceDumpCompleted = false;
        uiServicePumpCompleted = false;
        for (unsigned char i = eDisplayEventC0SetUp; i < eDisplayEventIceCreamMchOnOff; i++)
        {
            ceServiceSetState(uiP, _UI_ServiceState_ShowNTC, isEvent((teDisplayEvent)i));
        }
        ceServiceSetState(uiP, _UI_ServiceState_ShowNTC, isEvent(eDisplayEventSelectCabin));
        ceServiceSetState(uiP, _UI_ServiceState_ShowNTC, isEvent(eDisplayEventSetCabin));
#else
        for (unsigned char i = eDisplayEventC0SetUp; i < eDisplayEventIceCreamMchOnOff; i++)
        {
            ceServiceSetState(uiP, _UI_ServiceState_DispVersionRevision, isEvent((teDisplayEvent)i));
        }
        ceServiceSetState(uiP, _UI_ServiceState_VersionRevision, isEvent(eDisplayEventSelectCabin));
        ceServiceSetState(uiP, _UI_ServiceState_VersionRevision, isEvent(eDisplayEventSetCabin));
#endif  //HOSHI H30 and ALPS H10/30
        break;
    }

    ceBuzzer(isEvent(eDisplayEventBuzzer));
    clearEvent();
}
#endif //SERVICEMODEavailable