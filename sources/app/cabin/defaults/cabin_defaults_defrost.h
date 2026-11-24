#ifndef _CabinDefaults_Defrost_H
#define _CabinDefaults_Defrost_H

// FF Defrost Parameters' Defaults
#define _Defrost_FFAir_MaxRealTime_QuickFRZ  480 //minute
#define _Defrost_FFAir_MaxWorkTime_DO  240 //minute
#define _Defrost_FFAir_MaxWorkTime_DC  240 //minute
#define _Defrost_FFAir_MaxCycle_DO 3
#define _Defrost_FFAir_MaxCycle_DC 3
#define _Defrost_FFAir_DefrostCutOutTemp 70  // C Degree
#define _Defrost_FFAir_MinNoCoolTime 40 // minute
#define _Defrost_FFAir_MaxNoCoolTime 130 // minute
#define _Defrost_FFAir_ErrorMaxNoCoolTime 40 // minute

#define _Defrost_FFEva_MaxRealTime_QuickFRZ  480 //minute
#define _Defrost_FFEva_MaxWorkTime_DO  240 //minute
#define _Defrost_FFEva_MaxWorkTime_DC  240 //minute
#define _Defrost_FFEva_DefrostCutOutTemp 40  // C Degree
#define _Defrost_FFEva_MaxNoCoolTime 130 // minute
#define _Defrost_FFEva_ErrorMaxNoCoolTime 40 // minute

#define _Defrost_FFAirEva_MaxRealTime_QuickFRZ  480 //minute
#define _Defrost_FFAirEva_MaxRealTime_DO  360 //minute
#define _Defrost_FFAirEva_MaxRealTime_DC  1000 //minute
#define _Defrost_FFAirEva_MaxCycle_DO 4
#define _Defrost_FFAirEva_MaxCycle_DC 10
#define _Defrost_FFAirEva_DefrostCutOutTemp 40 // C Degree
#define _Defrost_FFAirEva_MaxNoCoolTime 130 // minute
#define _Defrost_FFAirEva_ErrorMaxCycle 6
#define _Defrost_FFAirEva_ErrorMaxNoCoolTime 40 // minute

// FRZ Defrost Parameters' Defaults
#define _Defrost_FRZEva_MaxRealTime_DO (26*60) // minute
#define _Defrost_FRZEva_MaxRealTime_DC (50*60) // minute
#define _Defrost_FRZEva_MaxQuickTime   (10*60) // minute
#define _Defrost_FRZEva_MaxWorkTime_DO (11*60) // minute
#define _Defrost_FRZEva_DefrostCutOutTemp 100  // C Degree
#define _Defrost_FRZEva_MaxDefrostTime_DO 60 // minute
#define _Defrost_FRZEva_MaxDefrostTime_DC 5 // minute
#define _Defrost_FRZEva_ErrorMaxDefrostTime 20 // minute

#define _Defrost_FRZAirEva_MaxRealTime_DO (26*60) // minute
#define _Defrost_FRZAirEva_MaxRealTime_DC (50*60) // minute
#define _Defrost_FRZAirEva_MaxQuickTime   (10*60) // minute
#define _Defrost_FRZAirEva_MaxWorkTime_DO (11*60) // minute
#define _Defrost_FRZAirEva_DefrostCutOutTemp 100  // C Degree
#define _Defrost_FRZAirEva_MaxDefrostTime_DO 60 // minute
#define _Defrost_FRZAirEva_MaxDefrostTime_DC 5 // minute
#define _Defrost_FRZAirEva_ErrorMaxDefrostTime 20 // minute
#define _Defrost_FRZAirEva_MinDefrostCutOutTemp 10  // C Degree


// JKR Defrost Parameters' Defaults
#define _Defrost_JKRAirEva_MaxRealTime_DO (26*60) // minute
#define _Defrost_JKRAirEva_MaxRealTime_DC (50*60) // minute
#define _Defrost_JKRAirEva_MaxQuickTime   (10*60) // minute
#define _Defrost_JKRAirEva_MaxWorkTime_DO (11*60) // minute
#define _Defrost_JKRAirEva_DefrostCutOutTemp 100  // C Degree
#define _Defrost_JKRAirEva_MaxDefrostTime_DO 60 // minute
#define _Defrost_JKRAirEva_MaxDefrostTime_DC 5 // minute
#define _Defrost_JKRAirEva_ErrorMaxDefrostTime 20 // minute

#endif
