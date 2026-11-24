#ifndef _CabinStates_HeaterC_H
#define _CabinStates_HeaterC_H

#define isHeaterCNone(_val) ((_val > _HeaterCNone_START)&& (_val < _HeaterCNone_END))
#define isHeaterCOff(_val)  ((_val > _HeaterCOff_START) && (_val < _HeaterCOff_END))
#define isHeaterCOn(_val)   ((_val > _HeaterCOn_START)  && (_val < _HeaterCOn_END))

typedef enum{
  _HeaterCNone_START,
  _HeaterCNone_ALL,
  _HeaterCNone_FLAPChannel,
  _HeaterCNone_END,
  
  _HeaterCOff_START,
  _HeaterCOff_FLAPChannel_DuringOffTime,
  _HeaterCOff_ALL_DuringOffTime,  
  _HeaterCOff_ALL_AtDoorsClosedState,
  _HeaterCOff_HotStart,
  _HeaterCOff_END,
  
  _HeaterCOn_START,
  _HeaterCOn_FLAPChannel_DuringOnTime,
  _HeaterCOn_ALL_DuringOnTime,
  _HeaterCOn_ALL_Always,
  _HeaterCOn_END,
}enHeaterCStates;

#endif
