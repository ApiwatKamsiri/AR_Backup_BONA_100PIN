#ifndef CONNECTED_CONFIGS_H
#define CONNECTED_CONFIGS_H

#define TIMEOPTIONTABLESIZE 9


typedef struct {
  
  struct {
    unsigned char brand;
    unsigned long long stockCode;
    unsigned char machineID;
    unsigned char nbVersion;
    unsigned char nbRevision;
    unsigned char wgVersion;
    unsigned char wgRevision;
    unsigned char productVersion;
    unsigned char dateDay;
    unsigned char dateMonth;
    unsigned char dateYear;
  }definitions;
  
  struct {
    unsigned char connectedAppAct:1;
    unsigned char swapConnButtons:1;
  }funcConnectedApp;
  
  struct {
    unsigned char logAct:1;
  }funcLog;  
  
  struct {
    unsigned char showHighTempErrorAct:1;
  }funcShowHighTempError;    
  
  struct {
    unsigned char delayFRZDefrostAct:1;
    signed int timeOptionTableF [TIMEOPTIONTABLESIZE];
    unsigned int minDefrostPeriod;
  }funcdelayDefrost;  
               
}stConnectedAppliance_Flash;



#endif