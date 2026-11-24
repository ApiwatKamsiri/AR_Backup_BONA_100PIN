#ifndef _CabinStates_HeaterB_H
#define _CabinStates_HeaterB_H

#define isHeaterBNone(_val) ((_val > _HeaterBNone_START)&& (_val < _HeaterBNone_END))
#define isHeaterBOff(_val)  ((_val > _HeaterBOff_START) && (_val < _HeaterBOff_END))
#define isHeaterBOn(_val)   ((_val > _HeaterBOn_START)  && (_val < _HeaterBOn_END))

typedef enum{
  _HeaterBNone_START,
  _HeaterBNone_ALL,
  _HeaterBNone_FLAPFlap,
  _HeaterBNone_FRZDefrost,
  _HeaterBNone_JKRDefrost,
  _HeaterBNone_END,
  
  _HeaterBOff_START,
  _HeaterBOff_FRZDefrost_Always,
  _HeaterBOff_JKRDefrost_Always,
  _HeaterBOff_FLAPFlap_DuringOffTime,
  _HeaterBOff_ALL_DuringOffTime,
  _HeaterBOff_HotStart,  
  _HeaterBOff_END,  
  
  _HeaterBOn_START,
  _HeaterBOn_FRZDefrost_Always,
  _HeaterBOn_JKRDefrost_Always,
  _HeaterBOn_FLAPFlap_DuringOnTime,
  _HeaterBOn_ALL_DuringOnTime,  
  _HeaterBOn_END,  
}enHeaterBStates;

#endif
