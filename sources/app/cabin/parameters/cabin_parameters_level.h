#ifndef _CabinParameters_Level_H
#define _CabinParameters_Level_H

#define _lowDutyDT 2
#define _medDutyDT 6
#define _hghDutyDT 2

#define _ambientDt 3
#define _ambientLevel 4

#define _compressorHotStartLevel 5
#define _compressorDefrostLevel  4

typedef struct{
  unsigned char low;
  unsigned char hgh;
}stDuty;

typedef struct{
  unsigned char lowDuty[ _lowDutyDT ];
  unsigned char medDuty[ _medDutyDT ];
  unsigned char hghDuty[ _hghDutyDT ];
}stDt;// gasLevels <= 10

typedef struct{
  unsigned char dt[ _AmbientSize ][ _ambientDt ];
}stAmbientDt;

typedef struct{
  unsigned char lowDuty[ _lowDutyDT + 1 ];
  unsigned char medDuty[ _medDutyDT + 1 ];
  unsigned char hghDuty[ _hghDutyDT + 1 ];
}stLevel;// gasLevels <= 10

typedef struct{
  unsigned char  level[ _AmbientSize ][ _ambientLevel ];
}stAmbientLevel;

typedef struct{
  stDuty dutyFF;
  stDuty dutyFRZ;
  
  stDt dtFF;
  stDt dtFRZ;
  
  stLevel levelFanFF;
  stLevel levelFanFRZ;

  stLevel levelGasFF;
  stLevel levelGasFRZ;
}stLevelParameters;

typedef struct{
  stAmbientDt FF;
  stAmbientDt FRZ;
  
  stAmbientLevel FanFF;
  stAmbientLevel FanFRZ;

  stAmbientLevel GasFF;
  stAmbientLevel GasFRZ;
}stAmbientLevelParameters;

#endif
