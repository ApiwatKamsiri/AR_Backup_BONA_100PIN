/** @file       EEProm_log_map.h
 *  @brief      Header file of EEProm_log_map device
 *  @copyright  (c) 2023-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammad Junaid Iqbal
 *  @date       24.07.2024
 */
#ifndef FILE_EEPROM_LOG_MAP_H
#define FILE_EEPROM_LOG_MAP_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_EEPROM_LOG_MAP_C
#define INTERFACE extern
#else
#define INTERFACE
#endif



typedef struct
{
    //4 bytes
    unsigned char eep_uc_ErrLog_1;// Failure details 1  0x0010
   // unsigned char eep_uc_ErrLog_2;// Failure details 2  0x0011
  //  unsigned char eep_uc_ErrLog_3;// Failure details 3  0x0012
   // unsigned char eep_uc_ErrLog_4;// Failure details 4  0x0013

     struct
    {
        unsigned char eep_uc_ErrOfDay_1;// Failure details 1  0x0014
      //  unsigned char eep_uc_ErrOfDay_2;// Failure details 2  0x0015
      //  unsigned char eep_uc_ErrOfDay_3;// Failure details 3  0x0016
    //    unsigned char eep_uc_ErrOfDay_4;// Failure details 4  0x0017
    }number_of_days;
    
}Failure_history;

typedef struct
{
   unsigned char eep_uc_Log_Ftemp:7; // 252 bytes 0x0018 to 0x0113
   unsigned char FDoor_open:1;
   
    
}F_room_temperature;

typedef struct
{
  unsigned char eep_uc_Log_Rtemp:7; 
  unsigned char RDoor_open:1;
   
    
}R_room_temperature;

typedef struct
{
  unsigned char TC_temperature:7; 
  unsigned char ES operation:1;
   
    
}Outside_temperature;

typedef struct
{
  unsigned char IM_temperature:7; 
  unsigned char Human_Detect:1;
       
}IM_temperature;

typedef struct
{
  unsigned char FEvaporator_temperature:7; 
  unsigned char F_Defrost_flag:1;
       
}FEvaporator_temperature;

typedef struct
{
  unsigned char REvaporator_temperature:7; 
  unsigned char R_Defrost_flag:1;
       
}REvaporator_temperature;

typedef struct
{
  unsigned char Humidity_value:7; 
  unsigned char Shabbat_flag:1;
       
}Humidity;


typedef struct
{
  unsigned char F_Door:4; 
  unsigned char R_Door:4;
       
}Dooropen_Cumulativetime;


typedef struct
{
  unsigned char Operation_Failure:1; 
  unsigned char Operation_24h:1; 
  unsigned char Operation_Wifimode:1; 
  unsigned char Operation_Wifimode:1; 
  unsigned char Operation_Reset:1;
  unsigned char Operation_IM_Water:1;
  unsigned char Operation_IM_Fullice:1;
  unsigned char Operation_IM_M0de:1;
  
}Operation_Flag1;

typedef struct
{
  unsigned char Operation_Mute:1; 
  unsigned char Operation_QuickCool:1; 
  unsigned char Operation_QuickFreeze:1; 
  unsigned char Operation_WifiRoutno:1; 
  unsigned char Operation_RTemperaturecontrol:4;
  
}Operation_Flag2;

typedef struct
{
  unsigned char Operation_VacuumMode:1; 
  unsigned char Operation_QuickCool:1; 
  unsigned char Operation_QuickFreeze:1; 
  unsigned char Operation_WifiRoutno:1; 
  unsigned char Operation_RTemperaturecontrol:4;
  
}Operation_Flag3;


typedef struct
{
  unsigned char eep_uc_Log_VDMax; 
  unsigned char eep_uc_Log_VDMin; 
  
}eep_uc_Log_VD;  //Power supply voltage record

typedef struct
{
  unsigned char eep_us_LogSum_Fdoor_H; 
  unsigned char eep_us_LogSum_Fdoor_L; 

  unsigned char eep_us_LogSum_Rdoor_H;
  unsigned char eep_us_LogSum_Rdoor_L; 
  
}eep_us_LogSum_door;  //Number of door openings


typedef struct
{
  unsigned char eep_us_LogSum_PwrOnTm_H;  // F Defrost times.		
  unsigned char eep_us_LogSum_PwrOnTm_L; 

  
}eep_us_LogSum_PwrOnTm;  //Number of energized days.		

 unsigned char eep_uc_Log_hourcnt // Number of counts per 2 hr
   
   
typedef struct
{
  unsigned char eep_us_ErrHis_Old_type1_H; 		
  unsigned char eep_us_ErrHis_Old_type1_L; 		
                                           
  unsigned char eep_us_ErrHis_Old_type2_H; 		
  unsigned char eep_us_ErrHis_Old_type2_L; 		
                                           
  unsigned char eep_us_ErrHis_Old_type3_;  	
  unsigned char eep_us_ErrHis_Old_type3_;  	
                                           
  unsigned char eep_us_ErrHis_Old_type4_;  	
  unsigned char eep_us_ErrHis_Old_type4_;  	
                                           
  unsigned char eep_us_ErrHis_Old_type5_;  	
  unsigned char eep_us_ErrHis_Old_type5_;  	
                                           
  unsigned char eep_us_ErrHis_Old_type6_;  	
  unsigned char eep_us_ErrHis_Old_type6_;  	
                                           
  unsigned char eep_us_ErrHis_Old_type7_;  	
  unsigned char eep_us_ErrHis_Old_type7_;  	
                                           
  unsigned char eep_us_ErrHis_Old_type8_;  	
  unsigned char eep_us_ErrHis_Old_type8_;  	
                                           
  unsigned char eep_us_ErrHis_Old_type9_;  	
  unsigned char eep_us_ErrHis_Old_type9_;  	
                                           
  unsigned char eep_us_ErrHis_Old_type10_; 		
  unsigned char eep_us_ErrHis_Old_type10_; 		
                                           
  unsigned char eep_us_ErrHis_Old_type;  //
  unsigned char eep_us_ErrHis_Old_type;  //
                                           
  unsigned char eep_us_ErrHis_Old_type;  //
  unsigned char eep_us_ErrHis_Old_type;  //
                                           
  unsigned char eep_us_ErrHis_Old_type;  //
  unsigned char eep_us_ErrHis_Old_type;  //
                                           
  unsigned char eep_us_ErrHis_Old_type;  //
  unsigned char eep_us_ErrHis_Old_type;  //

}eep_us_ErrHis_Old;  //Number of energized days.	
   



/******************************************************/

#undef INTERFACE // Should not let this roam free

#endif // FILE_EEPROM_LOG_MAP_H
