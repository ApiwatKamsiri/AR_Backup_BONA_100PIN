#ifndef _CabinParameters_Performance_H
#define _CabinParameters_Performance_H

typedef struct{
  unsigned char timeEva1;
  unsigned char timeEva2;
  signed short    tempEva1;
  signed short    tempEva2;
  unsigned char timeAir;
  signed char   deltaTempAir;
}stPerformanceParameters;

#endif
