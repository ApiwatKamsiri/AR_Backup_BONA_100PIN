#include "ui_slides.h"


void uiSlideData(tsSlides* slides, unsigned char data, unsigned char index, teDisplayCabinDataTypes dataType)
{
    if (index < _MAX_SLIDES_COUNT && dataType < eDisplayCabinDataTypesCount)
    {
        slides->slide[index].data = data;
        slides->slide[index].dataType = dataType;
    }
}
