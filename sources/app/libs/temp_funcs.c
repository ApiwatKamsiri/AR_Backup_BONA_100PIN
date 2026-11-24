#include "temp_funcs.h"


static unsigned char getCurrentPointer(stRefrigerator_Ram *rfP, unsigned char cabinIndex);
static unsigned char getCurrentPointerCup(signed char CelciusValue);

unsigned char getSetValueFromPointer(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP, unsigned char ind)
{
    unsigned char result = 0;
    unsigned i = 0;
    unsigned y = 0;
    unsigned char tempvalue = 0;

    tempvalue = (unsigned char)getCelciusValue(rfP->Values.temperatureConvPointer[ind]);

    for (i = 3; i < 18; i++)
    {
        if (cabinSetsDatasTable_F[i].display == tempvalue)
        {
            if (rfP->Cabin[ind].sets.work.Mode == _CabinMode_FF)
            {
                for (y = 0; y < rfC->Cabin[ind].sets.configs.FF; y++)
                {
                    if (i == rfC->Cabin[ind].sets.indexes.FF[y])
                    {
                        result = y;
                        break;
                    }
                }
            }
            else if (rfP->Cabin[ind].sets.work.Mode == _CabinMode_FRZ)
            {
                if (rfP->Cabin[1].sets.user.functions.Convertible)
                {
                    for (y = 0; y < rfC->Cabin[ind].sets.configs.functions.Convertible; y++)
                    {
                        if (i == rfC->Cabin[ind].sets.indexes.functions.Convertible[y])
                        {
                            result = y;
                            break;
                        }
                    }
                }
                else if (rfP->Cabin[1].sets.user.functions.CabinOff)
                {
                    for (y = 0; y < rfC->Cabin[ind].sets.configs.functions.CabinOff; y++)
                    {
                        if (i == rfC->Cabin[ind].sets.indexes.functions.CabinOff)
                        {
                            result = y;
                            break;
                        }
                    }
                }
                else
                {
                    for (y = 0; y < rfC->Cabin[ind].sets.configs.FRZ; y++)
                    {
                        if (i == rfC->Cabin[ind].sets.indexes.FRZ[y])
                        {
                            result = y;
                            break;
                        }
                    }
                }
            }
            break;
        }
    }

    return result;
}

bool checkSetValueStatus(stRefrigerator_Ram *rfP, unsigned char cabinIndex)
{
    if (rfP->Values.tempType == _TempType_Celcius)
        return true;

    unsigned char pointer = getCurrentPointer(rfP, cabinIndex);

    if (fahrenheitCelciusTable.lookUpTable[pointer][1] == fahrenheitCelciusTable.lookUpTable[pointer + 1][1])
        return false;

    return true;
}

bool checkSetValueStatusUp(stRefrigerator_Ram *rfP, unsigned char cabinIndex)
{
    if (rfP->Values.tempType == _TempType_Celcius)
        return true;

    unsigned char pointer = getCurrentPointer(rfP, cabinIndex);

    if (fahrenheitCelciusTable.lookUpTable[pointer][1] == fahrenheitCelciusTable.lookUpTable[pointer - 1][1])
        return false;

    return true;
}

static unsigned char getCurrentPointer(stRefrigerator_Ram *rfP, unsigned char cabinIndex)
{

    return rfP->Values.temperatureConvPointer[cabinIndex];
}

void refreshTemperaturePointer(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    if (rfP->Values.tempType == _TempType_Celcius)
    {
        for (unsigned char i = 0; i < _NumOfCabin; i++)
        {

            if (rfP->Cabin[i].sets.work.Mode == _CabinMode_FRZ)
            {
                if (rfP->Cabin[i].sets.user.functions.Convertible)
                    rfP->Values.temperatureConvPointer[i] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[i].sets.indexes.functions.Convertible[rfP->Cabin[i].sets.user.Convertible]].display);
                else
                    rfP->Values.temperatureConvPointer[i] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[i].sets.indexes.FRZ[rfP->Cabin[i].sets.user.FRZ]].display);
            }
            else if (rfP->Cabin[i].sets.work.Mode == _CabinMode_FF)
            {
                rfP->Values.temperatureConvPointer[i] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[i].sets.indexes.FF[rfP->Cabin[i].sets.user.FF]].display); // getCurrentPointerC(RefrigeratorParams.sets.FF_Sets[RefrigeratorData.sets.FfVal]);
            }
        }
    }
}

unsigned char getCurrentPointerC(signed char CelciusValue)
{

    for (unsigned char i = 0; i < LOOK_UP_TABLE_COUNT; i++)
    {
        if (CelciusValue == fahrenheitCelciusTable.lookUpTable[i][1])
            return i;
    }

    return 0;
}
unsigned char getCurrentPointerCup(signed char CelciusValue)
{

    for (unsigned char i = LOOK_UP_TABLE_COUNT; i > 0; i--)
    {
        if (CelciusValue == fahrenheitCelciusTable.lookUpTable[i][1])
            return i;
    }

    return 0;
}

signed char getFahrenheitValue(unsigned char pointer)
{
    return fahrenheitCelciusTable.lookUpTable[pointer][0];
}

signed char getCelciusValue(unsigned char pointer)
{
    return fahrenheitCelciusTable.lookUpTable[pointer][1];
}

signed char celciusToFahnrenheight(signed char valueInC)
{
    return (signed char)((signed short)(((double)valueInC / 5) * 9) + 32);
}

unsigned char resetTemperaturePointerFup(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP, unsigned char cabinIndex)
{
    if (cabinIndex == 2)
        return rfP->Values.temperatureConvPointer[2] = getCurrentPointerCup(cabinSetsDatasTable_F[rfC->Cabin[2].sets.indexes.FRZ[rfP->Cabin[2].sets.user.FRZ]].display);
    else if (cabinIndex == 1)
        return rfP->Values.temperatureConvPointer[1] = getCurrentPointerCup(cabinSetsDatasTable_F[rfC->Cabin[1].sets.indexes.FRZ[rfP->Cabin[1].sets.user.FRZ]].display);
    else
        return rfP->Values.temperatureConvPointer[0] = getCurrentPointerCup(cabinSetsDatasTable_F[rfC->Cabin[0].sets.indexes.FF[rfP->Cabin[0].sets.user.FF]].display);
}

unsigned char resetTemperaturePointerF(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP, unsigned char cabinIndex)
{
    /*  if(cabinIndex)
    return rfP->Values.temperatureConvPointer[1] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[1].sets.indexes.FRZ[rfP->Cabin[1].sets.user.FRZ]].display);
  else
    return rfP->Values.temperatureConvPointer[0] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[0].sets.indexes.FF[rfP->Cabin[0].sets.user.FF]].display);
*/
    unsigned char result = 0;

    if (rfP->Cabin[cabinIndex].sets.work.Mode == _CabinMode_FRZ)
    {
        if (rfP->Cabin[cabinIndex].sets.user.functions.Convertible)
        {
            rfP->Values.temperatureConvPointer[cabinIndex] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[cabinIndex].sets.indexes.functions.Convertible[rfP->Cabin[cabinIndex].sets.user.Convertible]].display);
            result = rfP->Values.temperatureConvPointer[cabinIndex];
        }
        else
        {
            rfP->Values.temperatureConvPointer[cabinIndex] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[cabinIndex].sets.indexes.FRZ[rfP->Cabin[cabinIndex].sets.user.FRZ]].display);
            result = rfP->Values.temperatureConvPointer[cabinIndex];
        }
    }
    else if (rfP->Cabin[cabinIndex].sets.work.Mode == _CabinMode_FF)
    {
        rfP->Values.temperatureConvPointer[cabinIndex] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[cabinIndex].sets.indexes.FF[rfP->Cabin[cabinIndex].sets.user.FF]].display);
        result = rfP->Values.temperatureConvPointer[cabinIndex];
    }
    else
    {
        if (rfC->Cabin[cabinIndex].type == _CabinType_FRZ)
        {
            if (rfP->Cabin[cabinIndex].sets.user.functions.Convertible)
                rfP->Values.temperatureConvPointer[cabinIndex] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[cabinIndex].sets.indexes.functions.Convertible[rfP->Cabin[cabinIndex].sets.user.Convertible]].display);
            else
                rfP->Values.temperatureConvPointer[cabinIndex] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[cabinIndex].sets.indexes.FRZ[rfP->Cabin[cabinIndex].sets.user.FRZ]].display);
        }
        else if (rfC->Cabin[cabinIndex].type == _CabinType_FF)
        {
            rfP->Values.temperatureConvPointer[cabinIndex] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[cabinIndex].sets.indexes.FF[rfP->Cabin[cabinIndex].sets.user.FF]].display);
        }
        else
        {
            rfP->Values.temperatureConvPointer[cabinIndex] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[cabinIndex].sets.indexes.FRZ[rfP->Cabin[cabinIndex].sets.user.FRZ]].display);
        }
        result = rfP->Values.temperatureConvPointer[cabinIndex];
    }

    return result;
}

void defaultTemperaturePointerF(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP, unsigned char cabinIndex)
{
    /*  if(cabinIndex)
    rfP->Values.temperatureConvPointer[1] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[1].sets.indexes.FRZ[0]].display);
  else
    rfP->Values.temperatureConvPointer[0] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[0].sets.indexes.FF[0]].display);
*/
    if (rfP->Cabin[cabinIndex].sets.work.Mode == _CabinMode_FRZ)
    {
        rfP->Values.temperatureConvPointer[cabinIndex] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[cabinIndex].sets.indexes.FRZ[0]].display);
    }
    else if (rfP->Cabin[cabinIndex].sets.work.Mode == _CabinMode_FF)
    {
        rfP->Values.temperatureConvPointer[cabinIndex] = getCurrentPointerC(cabinSetsDatasTable_F[rfC->Cabin[cabinIndex].sets.indexes.FF[0]].display);
    }
}

void setpointerValue(stRefrigerator_Ram *rfP, unsigned char cabinIndex, signed char savedVal)
{
    rfP->Values.temperatureConvPointer[cabinIndex] = getCurrentPointerC(savedVal);
}

signed char celciusExtToCelcius(signed short valueInCExt)
{
    return (signed char)(valueInCExt / 10);
}