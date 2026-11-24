#ifndef CONNECTED_RAM_H
#define CONNECTED_RAM_H

#define TARIFFARRAY 3   // 3 tariff is avaialble

typedef union {
      unsigned char byte;
      struct{
        unsigned char flag_CheapSelectActive:1;
        unsigned char flag_CheapSelectTariff1:1;
        unsigned char flag_CheapSelectTariff2:1;
        unsigned char flag_CheapSelectTariff3:1;  
        unsigned char reserved:4;
      }flags;
}unCheapSelectFlags;

typedef union {
      unsigned char byte;
      struct{
        unsigned char flag_CheapConfigInternet:1;
        unsigned char flag_CheapConfigDeDeUser:1;
        unsigned char reserved:6;
      }bits;
}unCheapSelectConfig;

typedef struct {
  unsigned char hour;
  unsigned char minute;
  unsigned char duration;
}stCheap_Tariff;

typedef struct {
    unsigned char cDailyHour;
    unsigned char cDailyMin;
    unsigned char cDailyDay;
    unsigned char cDailyMonth;
    unsigned char cDailyYear;
}stDailytime;

typedef struct {
    stCheap_Tariff cheapSelect[TARIFFARRAY];
    unCheapSelectFlags  cCheapSelectFlag;
    unCheapSelectConfig cCheapSelectConfig;
    unsigned char cCheapSelectCountDown;
}stDeDefrost;

typedef union {
  stDeDefrost DeDefrost;
  unsigned char DeDefrostArray[sizeof(stDeDefrost)];
}unDeDefrost;

typedef struct {
  signed char highestTempRead;
  unsigned char HTE_WarningReason;
}stHighTempErr;

typedef struct {
  unsigned char energyEfficiencyDutyFF;
}stEnergyEfficiency;

typedef struct {
  unDeDefrost   fSGrid;
  stDailytime   dailyTime;
  stHighTempErr fHighTempError;
  stEnergyEfficiency  fEnergy;
}unConnectedAppliance_Ram;

#endif
