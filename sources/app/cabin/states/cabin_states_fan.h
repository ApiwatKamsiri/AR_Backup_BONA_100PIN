#ifndef _CabinStates_Fan_H
#define _CabinStates_Fan_H

#define isFanNone(_val) ((_val > _FanNone_START) && (_val < _FanNone_END))
#define isFanOff(_val) ((_val > _FanOff_START) && (_val < _FanOff_END))
#define isFanLevel(_val) ((_val > _FanLevel_START) && (_val < _FanLevel_END))
#define isFanError(_val) ((_val > _FanError_START) && (_val < _FanError_END))
#define isFanQuick(_val) ((_val > _FanQuick_START) && (_val < _FanQuick_END))
#define isFanHotStart(_val) ((_val > _FanHotStart_START) && (_val < _FanHotStart_END))

typedef enum
{
    _FanNone_START,
    _FanNone_FF,
    _FanNone_FRZ,
    _FanNone_JKR,
    _FanNone_FLAP,
    //_FanNone_BioFresh,
    _FanNone_END,

    _FanOff_START,
    _FanOff_FF_DutyInsideInterval,
    _FanOff_FF_AirLowerThanFanCutOut,
    _FanOff_FF_AtCompressorOFFAmbientLevel,
    _FanOff_FF_AtCompressorONFRZAmbientLevel,
    _FanOff_FF_Always,
    _FanOff_FF_AtDC,
    _FanOff_FRZ_Always,
    _FanOff_FRZ_AtCompressorOFFAmbientLevel,
    _FanOff_FRZ_AtCompressorONFFAmbientLevel,
    _FanOff_JKR_Always,
    _FanOff_JKR_AirLowerThanFanCutOut,
    _FanOff_FLAP_AirLowerThanFanCutOutDuringAirSuppliedAtFFMode,
    _FanOff_FLAP_DuringNoAirSuppliedAtFFMode,
    _FanOff_FLAP_Always,
    //_FanOff_BioFresh_AirLowerThanFanCutOutDuringAirSupplied,
    //_FanOff_BioFresh_DuringNoAirSupplied,
    //_FanOFF_BioFresh_Always,
    _FanOff_END,

    _FanError_START,
    //_FanError_BioFresh,
    _FanError_FF,
    _FanError_FLAP,
    _FanError_FRZ,
    _FanError_JKR,
    _FanError_END,

    _FanQuick_START,
    _FanQuick_FRZ,
    _FanQuick_END,

    _FanLevel_START,
    //_FanLevel_BioFresh_AirHigherThanFanCutOutDuringAirSupplied,
    _FanLevel_FF,
    _FanLevel_FF_VioledOn,
    _FanLevel_FF_AirHigherThanFanCutOut,
    _FanLevel_FF_AtDO_DutyOutsideInterval,
    _FanLevel_FF_AtDC_DutyOutsideInterval,
    _FanLevel_FF_AfterCoolState,
    _FanLevel_FF_AtCompressorOFFAmbientLevel,
    _FanLevel_FF_AtCompressorONFRZAmbientLevel,
    _FanLevel_FLAP_FFMode_AirHigherThanFanCutOutDuringAirSupplied,
    _FanLevel_FLAP_FRZMode,
    _FanLevel_FRZ_AtDO,
    _FanLevel_FRZ_AtDC,
    _FanLevel_FRZ_AtDO_EvaLowerThanFanCutIn,
    _FanLevel_FRZ_AtDC_EvaLowerThanFanCutIn,
    _FanLevel_FRZ_AfterCoolState,
    _FanLevel_FRZ_AtCompressorOFFAmbientLevel,
    _FanLevel_FRZ_AtCompressorONFFAmbientLevel,
    _FanLevel_JKR_FFMode_AtDO_AirHigherThanFanCutOut,
    _FanLevel_JKR_FFMode_AtDC_AirHigherThanFanCutOut,
    _FanLevel_JKR_FRZMode_AtDO,
    _FanLevel_JKR_FRZMode_EvaLowerThanFanCutIn,
    _FanLevel_FF_MaxIdleTimeElapsed,
    _FanLevel_FRZ_MaxIdleTimeElapsed,
    _FanLevel_FRZ_ConvertibleMode,
    _FanLevel_END,

    _FanHotStart_START,
    //_FanHotStart_BioFresh,
    _FanHotStart_FF,
    _FanHotStart_FLAP,
    _FanHotStart_FRZ,
    _FanHotStart_JKR,
    _FanHotStart_END,

    _FanCommercialStart_FLAP,                                                    //APW : Add CT heater software
    _FanCommercialStart_FF,                                                      //APW : Add CT heater software 
    _FanCommercialStart_FRZ,                                                     //APW : Add CT heater software
} enFanStates;

#endif
