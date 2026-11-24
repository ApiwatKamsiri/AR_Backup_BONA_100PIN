#include "ui_errors.h"
#include "ui_slides.h"
#include "dev/display_spi.h"

void uiErrorsControlSelected(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam* uiP = params->ui;
    stRefrigerator_Ram* rfP = params->rf;
    if (uiP->errors.errorsIndex >= rfP->Values.errors.errorsNumber)
    {
        uiP->slides.slideNumber = 0;
        return;
    }
    
    if (uiP->errors.errorsIndex == rfP->Values.errors.highTempErrNumber)
    {
        switch (rfP->Values.tempType)
        {
            case _TempType_Fahrenheit:
                uiSlideData(&uiP->slides, eDisplaySpecialsFDegree, 1, eDisplayCabinDataTypeSpecial);
                uiSlideData(&uiP->slides,rfP->Values.errors.errorsTable[uiP->errors.errorsIndex], 0, eDisplayCabinDataTypeNumber);
                uiP->slides.slideNumber = 2;
            break;

            case _TempType_Celcius:
                uiSlideData(&uiP->slides,eDisplaySpecialsCDegree, 1, eDisplayCabinDataTypeSpecial);
                uiSlideData(&uiP->slides,rfP->Values.errors.errorsTable[uiP->errors.errorsIndex], 0, eDisplayCabinDataTypeNumber);
                uiP->slides.slideNumber = 2;
            default: break;
        }
    }
    else if (uiP->errors.errorsIndex != rfP->Values.errors.highTempErrNumber)
    {
        uiSlideData(&uiP->slides, eDisplaySpecialsError, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, rfP->Values.errors.errorsTable[uiP->errors.errorsIndex], 1, eDisplayCabinDataTypeNumber);
        uiP->slides.slideNumber = 2;
    }
}
