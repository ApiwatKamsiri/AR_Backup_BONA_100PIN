#ifndef _CabinStates_Defrost_H
#define _CabinStates_Defrost_H

#define isDefrostNone(_val) ((_val > _DefrostNone_START) && (_val < _DefrostNone_END))
#define isDefrostIn(_val) ((_val > _DefrostIn_START) && (_val < _DefrostIn_END))
#define isDefrostOut(_val) ((_val > _DefrostOut_START) && (_val < _DefrostOut_END))

typedef enum
{
    _DefrostNone_START,
    _DefrostNone_FF,
    _DefrostNone_FF_ForIceCream,
    _DefrostNone_FRZ,
    _DefrostNone_FRZ_ForIceCream,
    _DefrostNone_JKR,
    _DefrostNone_JKR_ForIceCream,
    _DefrostNone_FLAP,
    //_DefrostNone_BioFresh,
    _DefrostNone_END,

    _DefrostIn_START,
    _DefrostIn_FF_MaxWorkTimeElapsed,
    _DefrostIn_FF_MaxRealTimeElapsed,
    _DefrostIn_FF_MaxCycleReached,
    _DefrostIn_FF_ErrorMaxCycleReached,
    _DefrostIn_FRZ_MaxRealTimeElapsed,
    _DefrostIn_FRZ_MaxWorkTimeElapsed,
    _DefrostIn_FRZ_MaxQuickTimeElapsed,
    _DefrostIn_FRZ_MaxDefrostHighTempTimeElapsed,
    _DefrostIn_JKR_MaxRealTimeElapsed,
    _DefrostIn_JKR_MaxWorkTimeElapsed,
    _DefrostIn_JKR_MaxQuickTimeElapsed,
    _DefrostIn_END,

    _DefrostOut_START,
    _DefrostOut_FF_MeanDefrostTimeElapsed,
    _DefrostOut_FF_AirHigherThanDefrostCutOutAndMinNoCoolTimeElapsed,
    _DefrostOut_FF_EvaHigherThanDefrostCutOut,
    _DefrostOut_FF_MaxNoCoolTimeElapsed,
    _DefrostOut_FF_ErrorMaxNoCoolTimeElapsed,
    _DefrostOut_FF_Always,
    _DefrostOut_FF_ForIceCream,
    _DefrostOut_FRZ_MeanDefrostTimeElapsed,
    _DefrostOut_FRZ_EvaHigherThanDefrostCutOut,
    _DefrostOut_FRZ_MaxDefrostTimeElapsed_DC,
    _DefrostOut_FRZ_MaxDefrostTimeElapsed_DO,
    _DefrostOut_FRZ_ErrorMaxDefrostTimeElapsed,
    _DefrostOut_FRZ_Always,
    _DefrostOut_FRZ_ForIceCream,
    _DefrostOut_JKR_MeanDefrostTimeElapsed,
    _DefrostOut_JKR_EvaHigherThanDefrostCutOut,
    _DefrostOut_JKR_MaxDefrostTimeElapsed_DC,
    _DefrostOut_JKR_MaxDefrostTimeElapsed_DO,
    _DefrostOut_JKR_ErrorMaxDefrostTimeElapsed,
    _DefrostOut_JKR_Always,
    _DefrostOut_JKR_ForIceCream,
    _DefrostOut_END,
} enDefrostStates;

#endif
