#ifndef _CabinStates_FLAP_H
#define _CabinStates_FLAP_H

#define isFlapNone(_val)        ((_val > _FlapNone_START)       && (_val < _FlapNone_END))
#define isFlapOff(_val)         ((_val > _FlapOff_START)        && (_val < _FlapOff_END))
#define isFlapPositionA(_val)   ((_val > _FlapPositionA_START)  && (_val < _FlapPositionA_END))
#define isFlapPositionB(_val)   ((_val > _FlapPositionB_START)  && (_val < _FlapPositionB_END))
#define isFlapPositionC(_val)   ((_val > _FlapPositionC_START)  && (_val < _FlapPositionC_END))

typedef enum
{
    _FlapNone_START,
    _FlapNone_ALL,
    _FlapNone_FLAP,
    //_FlapNone_BioFresh,
    _FlapNone_END,

    _FlapOff_START,
    _FlapOff_FLAP_AirLowerThanFlapCutOutAtDuringAirSuppliedAtFFMode,
    _FlapOff_FLAP_DuringNoAirSuppliedAtFFMode,
    _FlapOff_FLAP_Always,
    _FlapOff_ALL_DuringOffTime,
    _FlapOff_END,

    _FlapPositionA_START,
    _FlapPositionA_FLAP_AirHigherThanFlapCutOutAtDuringAirSuppliedAtFFMode,

    _FlapPositionA_FLAP_AlwaysAtFRZMode,
    _FlapPositionA_FLAP_Always,
    _FlapPositionA_ALL_DuringOnTime,
    _FlapPositionB_ALL_DuringOnTime, //Kyle???
    _FlapPositionA_END,

    _FlapPositionB_START,
    _FlapPositionB_END,
    _FlapPositionC_START,
    _FlapPositionC_END,
} enFlapStates;

#endif
