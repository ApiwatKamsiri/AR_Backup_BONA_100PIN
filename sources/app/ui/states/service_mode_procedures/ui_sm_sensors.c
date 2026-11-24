#include "ui_sm_sensors.h"
#include "app/libs/temp_funcs.h"
#include "../../procedures/ui_slides.h"

#ifdef SERVICEMODEavailable

static void SensorsCheckCooler(tsUiSmSensorsData *sens, stCooler_Flash const *coolerC, unsigned char no);
static void SensorsCheckCabin(tsUiSmSensorsData *sens, stCabin_Flash const *cabinC, unsigned char no);
static void SensorsCheckIceMachine(tsUiSmSensorsData *sens, stIceMachine_Flash const *iceMachineC, unsigned char no);
//static void SensorsCheckIceCreamMachine(tsUiSmSensorsData *sens, stIceCreamMachine_Flash const *iceCreamMachineC, unsigned char no);

void uiSmSensCheckAll(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiSmSensorsData *sens = &params->ui->states.service.sens;

    sens->number = 0;
    for (unsigned char i = 0; i < _SizeOfSensorsCheckTable; i++)
        sens->checkTable[i] = 0;

    SensorsCheckCooler(sens, &consts->rf->Cooler, 0);
    for (unsigned char i = 0; i < _NumOfCabin; i++)
        SensorsCheckCabin(sens, &consts->rf->Cabin[i], i);

    SensorsCheckIceMachine(sens, &consts->rf->IceMachine, 0);
    //SensorsCheckIceCreamMachine(sens, &consts->rf->IceCreamMachine, 0);

    for (unsigned char i = 0; i < _SizeOfSensorsCheckTable; i++)
    {
        if (sens->checkTable[i])
        {
            for (unsigned char j = 0; j < _NumOfCabin; j++)
            {
                if (sens->checkTable[i] & (0x01 << j))
                {
                    sens->table[sens->number][0] = i;
                    sens->table[sens->number][1] = j;
                    sens->number++;
                }
            }
        }
    }
}

void uiSmSensControlSelected(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiSmSensorsData *sens = &params->ui->states.service.sens;
    tsUiRam *uiP = params->ui;

    if (sens->index >= sens->number)
    {
        params->ui->slides.slideIndex = 0;
        return;
    }

    unsigned char sensType = sens->table[sens->index][0];
    unsigned char sensIndex = sens->table[sens->index][1];

    switch (sensType)
    {
    case _Sensor_FF_Air:
        uiSlideData(&uiP->slides, eDisplaySpecialsFFAir, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, sensIndex, 2, eDisplayCabinDataTypeNumber);
        uiSlideData(&uiP->slides, (unsigned char)celciusExtToCelcius(params->rf->Cabin[sensIndex].components.airTempSensor.valueInCExt), 3, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 4;
        break;

    case _Sensor_FF_Eva:
        uiSlideData(&uiP->slides, eDisplaySpecialsFFEva, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, sensIndex, 2, eDisplayCabinDataTypeNumber);
        uiSlideData(&uiP->slides, (unsigned char)celciusExtToCelcius(params->rf->Cabin[sensIndex].components.evaTempSensor.valueInCExt), 3, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 4;
        break;

    case _Sensor_FRZ_Air:
        uiSlideData(&uiP->slides, eDisplaySpecialsFRZAir, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, sensIndex, 2, eDisplayCabinDataTypeNumber);
        uiSlideData(&uiP->slides, (unsigned char)celciusExtToCelcius(params->rf->Cabin[sensIndex].components.airTempSensor.valueInCExt), 3, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 4;
        break;

    case _Sensor_FRZ_Eva:
        uiSlideData(&uiP->slides, eDisplaySpecialsFRZEva, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, sensIndex, 2, eDisplayCabinDataTypeNumber);
        uiSlideData(&uiP->slides, (unsigned char)celciusExtToCelcius(params->rf->Cabin[sensIndex].components.evaTempSensor.valueInCExt), 3, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 4;
        break;

    case _Sensor_JKR_Air:
        uiSlideData(&uiP->slides, eDisplaySpecialsJKRAir, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, sensIndex, 2, eDisplayCabinDataTypeNumber);
        uiSlideData(&uiP->slides, (unsigned char)celciusExtToCelcius(params->rf->Cabin[sensIndex].components.airTempSensor.valueInCExt), 3, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 4;
        break;

    case _Sensor_JKR_Eva:
        uiSlideData(&uiP->slides, eDisplaySpecialsJKREva, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, sensIndex, 2, eDisplayCabinDataTypeNumber);
        uiSlideData(&uiP->slides, (unsigned char)celciusExtToCelcius(params->rf->Cabin[sensIndex].components.evaTempSensor.valueInCExt), 3, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 4;
        break;

    case _Sensor_FLAP_Air:
        uiSlideData(&uiP->slides, eDisplaySpecialsFLAPAir, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, sensIndex, 2, eDisplayCabinDataTypeNumber);
        uiSlideData(&uiP->slides, (unsigned char)celciusExtToCelcius(params->rf->Cabin[sensIndex].components.airTempSensor.valueInCExt), 3, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 4;
        break;

    case _Sensor_Cooler_AmbinetSensor:
        uiSlideData(&uiP->slides, eDisplaySpecialsCoolerAmbientSensor, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, (unsigned char)celciusExtToCelcius(params->rf->Cooler.components.ambientTempSensor.valueInCExt), 1, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 2;
        break;

    case _Sensor_Cooler_HumiditySensor:
        uiSlideData(&uiP->slides, eDisplaySpecialsCoolerHumiditySensor, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, (unsigned char)(params->rf->Cooler.components.humiditySensor.humidityValue), 1, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 2;
        break;

    /*
    case _Sensor_BioFresh_Air:
        uiSlideData(&uiP->slides, eDisplaySpecialsBioFresh_Air, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, eDisplaySpecialsCabin, 1, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, sensIndex, 2, eDisplayCabinDataTypeNumber);
        uiSlideData(&uiP->slides, (unsigned char)celciusExtToCelcius(params->rf->Cabin[sensIndex].components.airTempSensor.valueInCExt), 3, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 4;
    break;
    */
    case _Sensor_IceMachine_CaseTemp:
        uiSlideData(&uiP->slides, eDisplaySpecialsIceMachineTempSensor, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, (unsigned char)celciusExtToCelcius(params->rf->IceMachine.components.caseTempSensor.valueInCExt), 1, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 2;
        break;

    /*case _Sensor_IceCreamMachine_Temp:
        uiSlideData(&uiP->slides, eDisplaySpecialsIceCreamMachineTempSensor, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, (unsigned char)celciusExtToCelcius(params->rf->IceCreamMachine.components.tempSensor.valueInCExt), 1, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 2;
        break;*/

    default:;
        break;
    }
}

static void SensorsCheckCooler(tsUiSmSensorsData *sens, stCooler_Flash const *coolerC,
                        unsigned char no)
{
    if (no >= 8)
        return;
    if (coolerC->components.ambientTempSensor.type == _TempSensorType_NTCwithADC || coolerC->components.ambientTempSensor.type == _TempSensorType_NTC_Ambient_ADC )
        sens->checkTable[_Sensor_Cooler_AmbinetSensor] |= 0x01;
    else if (coolerC->components.ambientTempSensor.type == _TempSensorType_SHT3X)
    {
        sens->checkTable[_Sensor_Cooler_AmbinetSensor] |= 0x01;
        sens->checkTable[_Sensor_Cooler_HumiditySensor] |= 0x01;
    }
}

static void SensorsCheckCabin(tsUiSmSensorsData *sens, stCabin_Flash const *cabinC,
                       unsigned char no)
{
    if (no >= 8)
        return;
    switch (cabinC->type)
    {
    case _CabinType_FF:
        if (cabinC->components.airTempSensor.type)
            sens->checkTable[_Sensor_FF_Air] |= 0x01 << no;
        if (cabinC->components.evaTempSensor.type)
            sens->checkTable[_Sensor_FF_Eva] |= 0x01 << no;
        break;

    case _CabinType_FRZ:
        if (cabinC->components.airTempSensor.type)
            sens->checkTable[_Sensor_FRZ_Air] |= 0x01 << no;
        if (cabinC->components.evaTempSensor.type)
            sens->checkTable[_Sensor_FRZ_Eva] |= 0x01 << no;
        break;

    case _CabinType_JKR:
        if (cabinC->components.airTempSensor.type)
            sens->checkTable[_Sensor_JKR_Air] |= 0x01 << no;
        if (cabinC->components.evaTempSensor.type)
            sens->checkTable[_Sensor_JKR_Eva] |= 0x01 << no;
        break;
    case _CabinType_FLAP:
        if (cabinC->components.airTempSensor.type)
            sens->checkTable[_Sensor_FLAP_Air] |= 0x01 << no;
        break;
        /*case _CabinType_BioFRESH:
            if (cabinC->components.airTempSensor.type)
                sens->checkTable[_Sensor_BioFresh_Air] |= 0x01 << no;
        break;*/

    case _CabinType_NONE:
    default:;
        break;
    }
}

static void SensorsCheckIceMachine(tsUiSmSensorsData *sens, stIceMachine_Flash const *iceMachineC,
                            unsigned char no)
{
    if (no >= 8)
        return;

    if (iceMachineC->type != _IceMachineType_NONE)
    {
        if (iceMachineC->components.caseTempSensor.type)
            sens->checkTable[_Sensor_IceMachine_CaseTemp] |= 0x01 << no;
    }
}

/*static void SensorsCheckIceCreamMachine(tsUiSmSensorsData *sens, stIceCreamMachine_Flash const *iceCreamMachineC,
                                 unsigned char no)
{
    if (no >= 8)
        return;

    if (iceCreamMachineC->type != _IceCreamMachineType_NONE)
    {
        if (iceCreamMachineC->components.tempSensor.type)
            sens->checkTable[_Sensor_IceCreamMachine_Temp] |= 0x01 << no;
    }
}*/
#endif //SERVICEMODEavailable
