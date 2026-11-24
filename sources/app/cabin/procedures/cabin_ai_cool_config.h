#ifndef CABIN_AI_COOL_CONFIG_H
#define CABIN_AI_COOL_CONFIG_H

// AI Cool configuration macros and constants
// This file contains only macro definitions, no function prototypes
// to avoid circular include dependencies

#if defined (AI_COOL_V1)

#define NUMBER_OF_AI_ANALYZE_DAYS  1//3//7
#define NUMBER_OF_AI_ANALYZE_HOURS 24

#define COMPRESSOR_CMP_VAL_SIZE 6

#define _CMP_AMB0_L1_L 32//45
#define _CMP_AMB0_L1_H 55//75
#define _CMP_AMB0_L2_L 43//70
#define _CMP_AMB0_L2_H 65//105
#define _CMP_AMB0_L3_L 55//100
#define _CMP_AMB0_L3_H 90//135
                       //
#define _CMP_AMB1_L1_L 32//45
#define _CMP_AMB1_L1_H 55//75
#define _CMP_AMB1_L2_L 43//70
#define _CMP_AMB1_L2_H 65//105
#define _CMP_AMB1_L3_L 55//100
#define _CMP_AMB1_L3_H 90//135
                       //
#define _CMP_AMB2_L1_L 45//45
#define _CMP_AMB2_L1_H 65//75
#define _CMP_AMB2_L2_L 55//70
#define _CMP_AMB2_L2_H 85//105
#define _CMP_AMB2_L3_L 80//100
#define _CMP_AMB2_L3_H 110//135
                       //
#define _CMP_AMB3_L1_L 45//45
#define _CMP_AMB3_L1_H 75//75
#define _CMP_AMB3_L2_L 68//70
#define _CMP_AMB3_L2_H 90//105
#define _CMP_AMB3_L3_L 75//100
#define _CMP_AMB3_L3_H 128//135
                       //
#define _CMP_AMB4_L1_L 65//45
#define _CMP_AMB4_L1_H 90//75
#define _CMP_AMB4_L2_L 75//70
#define _CMP_AMB4_L2_H 110//105
#define _CMP_AMB4_L3_L 90//100
#define _CMP_AMB4_L3_H 128//135

#define _AI_COMP_ON_TIME_CMP_VAL 28

#define _CMP_AMB0_CONST_AI_SPEED 80
#define _CMP_AMB1_CONST_AI_SPEED 45
#define _CMP_AMB2_CONST_AI_SPEED 50
#define _CMP_AMB3_CONST_AI_SPEED 58
#define _CMP_AMB4_CONST_AI_SPEED 110

#define _AI_FF_SET 5
#define _AI_FRZ_SET 240 //-16

#define _AI_COOL_FF_AIR_CUT_IN_TEMP 61
#define _AI_COOL_FF_AIR_CUT_OUT_TEMP 58

#define _AI_COOL_FRZ_AIR_CUT_IN_TEMP -151
#define _AI_COOL_FRZ_AIR_CUT_OUT_TEMP -153

#define _AI_LABEL_INC_WORKTIME 30
#define _AI_LABEL_DEC_WORKTIME 10

#define _AI_SPEED_CHANGE_TIME 2*60 //min

#define _AI_CUTOUT_DISTANCE_MAX 50
#define _AI_CUTOUT_DISTANCE_MIN 0

#define _AI_COMPRESSOR_SPEED_MIN 32
#define _AI_COMPRESSOR_SPEED_MAX 128

#endif // AI_COOL_V1

#endif // CABIN_AI_COOL_CONFIG_H
