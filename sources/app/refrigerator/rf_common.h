#ifndef RF_COMMON_H
#define RF_COMMON_H


#define _NumOfCabin 2 //5



typedef struct
{
  unsigned short No[3];
  unsigned char Version;
  unsigned char Revision;
  unsigned char CoreVersion;
  unsigned char CoreRevision;
} stStock;


/// SAK
typedef union{
  struct{
    unsigned char rgbLEDUsage:1;
    unsigned char HumiditySensor:1;
    unsigned char HarvestFresh:1;
    unsigned char ZeroStarDisplay:1;
    unsigned char FreshMeterDisplay:1;
  }device;
  unsigned char allDevices;
}unI2CDevices;


#define __RF_OF(_type, _memberPtr, _member) ((_type *)((char *)(_memberPtr) - offsetof(_type, _member)))

#endif