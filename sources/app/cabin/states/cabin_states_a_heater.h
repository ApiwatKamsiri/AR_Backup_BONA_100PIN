#ifndef _CabinStates_HeaterA_H
#define _CabinStates_HeaterA_H

#define isHeaterANone(_val) ((_val > _HeaterANone_START)&& (_val < _HeaterANone_END))
#define isHeaterAOff(_val)  ((_val > _HeaterAOff_START) && (_val < _HeaterAOff_END))
#define isHeaterAOn(_val)   ((_val > _HeaterAOn_START)  && (_val < _HeaterAOn_END))

typedef enum{
  _HeaterANone_START,
  _HeaterANone_ALL,
  _HeaterANone_FFLowTemp,  
  _HeaterANone_JKRLowTemp,
  _HeaterANone_FLAPLowTemp,
  _HeaterANone_END,
	
  _HeaterAOff_START,
  _HeaterAOff_FFLowTemp_AirHigherThanLTHCutOut,
  _HeaterAOff_FFLowTemp_Always,
  _HeaterAOff_FFLowTemp_MaxNoCoolTimeNotElapsed,
  _HeaterAOff_JKRLowTemp_AirHigherThanLTHCutOut,
  _HeaterAOff_JKRLowTemp_Always, 
  _HeaterAOff_FLAPLowTemp_AirHigherThanLTHCutOut, 
  _HeaterAOff_FLAPLowTemp_Always,   
  _HeaterAOff_ALL_DuringOffTime,
  _HeaterAOff_HotStart,  
  _HeaterAOff_END,

  _HeaterAOn_START, 
  _HeaterAOn_FFLowTemp_ThereIsAFRZDefrost,
  _HeaterAOn_FFLowTemp_AirLowerThanLTHCutIn,
  _HeaterAOn_FFLowTemp_MaxNoCoolTimeElapsed, 
  _HeaterAOn_JKRLowTemp_ThereIsAFRZDefrost,  
  _HeaterAOn_JKRLowTemp_AirLowerThanLTHCutIn, 
  _HeaterAOn_FLAPLowTemp_AirLowerThanLTHCutIn,  
  _HeaterAOn_ALL_DuringOnTime,
  _HeaterAOn_END,
}enHeaterAStates;
#endif
