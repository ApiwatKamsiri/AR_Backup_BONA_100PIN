#ifndef _Refrigerator_AI_COOL_H
#define _Refrigerator_AI_COOL_H


#if defined(AI_COOL_V1_5)

#define A_COOL_VERSION 2

#define NUMBER_OF_AI_ANALYZE_HOURS 24

#define NUMBER_OF_PERSONAS 4
#define NUMBER_OF_FEATURES 6

#define AI_CABIN_SIZE 2

#define FF_OFFSET 0
#define FRZ_OFFSET 1

#define _AI_FF_CRITICAL_SET_INDEX 8  // set index value of corrosponding referance set value in cabinSetsDatasTable_F
#define _AI_FRZ_CRITICAL_SET_INDEX 16

#define FF_OCCASIONAL_SNACKER_OFFSET 8 // offset value of corrosponding referance set value in cabinSetsDatasTable_F 0.8 C
#define FF_BUSY_PROFESSIONAL_OFFSET 4
#define FF_BALANCED_HOUSEHOLD_OFFSET 0
#define FF_FAMILY_MATRIARCH_CHEF_OFFSET 0

#define FRZ_OCCASIONAL_SNACKER_OFFSET 4
#define FRZ_BUSY_PROFESSIONAL_OFFSET 2
#define FRZ_BALANCED_HOUSEHOLD_OFFSET 0
#define FRZ_FAMILY_MATRIARCH_CHEF_OFFSET 0

#define FRZ_DEFROST_OCCASIONAL_SNACKER_OFFSET 24*60 // defrost time offset value for each persona
#define FRZ_DEFROST_BUSY_PROFESSIONAL_OFFSET 12*60
#define FRZ_DEFROST_BALANCED_HOUSEHOLD_OFFSET 0
#define FRZ_DEFROST_FAMILY_MATRIARCH_CHEF_OFFSET 0

#define AI_COMPRESSOR_ONTIMER_THRESHOLD 28
#define NUMBER_OF_AMBIENTS 5

#define AI_AMB0_OCCASIONAL_SNACKER_COMP_SPEED 32
#define AI_AMB1_OCCASIONAL_SNACKER_COMP_SPEED 32
#define AI_AMB2_OCCASIONAL_SNACKER_COMP_SPEED 40
#define AI_AMB3_OCCASIONAL_SNACKER_COMP_SPEED 50
#define AI_AMB4_OCCASIONAL_SNACKER_COMP_SPEED 100

#define AI_AMB0_BUSY_PROFESSIONAL_COMP_SPEED 32
#define AI_AMB1_BUSY_PROFESSIONAL_COMP_SPEED 32
#define AI_AMB2_BUSY_PROFESSIONAL_COMP_SPEED 40
#define AI_AMB3_BUSY_PROFESSIONAL_COMP_SPEED 50
#define AI_AMB4_BUSY_PROFESSIONAL_COMP_SPEED 100

#define AI_AMB0_BALANCED_HOUSEHOLD_COMP_SPEED 32
#define AI_AMB1_BALANCED_HOUSEHOLD_COMP_SPEED 32
#define AI_AMB2_BALANCED_HOUSEHOLD_COMP_SPEED 48
#define AI_AMB3_BALANCED_HOUSEHOLD_COMP_SPEED 57
#define AI_AMB4_BALANCED_HOUSEHOLD_COMP_SPEED 110

#define AI_AMB0_FAMILY_MATRIARCH_CHEF_COMP_SPEED 40
#define AI_AMB1_FAMILY_MATRIARCH_CHEF_COMP_SPEED 40
#define AI_AMB2_FAMILY_MATRIARCH_CHEF_COMP_SPEED 57
#define AI_AMB3_FAMILY_MATRIARCH_CHEF_COMP_SPEED 70
#define AI_AMB4_FAMILY_MATRIARCH_CHEF_COMP_SPEED 110


typedef enum
{
    _PERSONA_OCCASIONAL_SNACKER= 0,
    _PERSONA_BUSY_PROFESSIONAL,
    _PERSONA_BALANCED_HOUSEHOLD,
    _PERSONA_FAMILY_MATRIARCH_CHEF,
}enPersonaAI;

typedef enum{
    _BEFORE_YESTERDAY = 0,
    _YESTERDAY,
    _TODAY,
    _ENDDAY,
}enDay;

typedef struct 
{
    unsigned char openCount;
    unsigned int  openTime[NUMBER_OF_AI_ANALYZE_HOURS];
}stDoorInfo;

typedef struct
{
    unsigned char totalDoorOpenCount;
    unsigned char meanDoorOpenTime;
    unsigned char medianDoorOpenTime;
    unsigned char stdDevDoorOpenTime;
    unsigned int maximumDoorOpenTime;
    unsigned int totalDoorOpenTime;
}stFeatures;

typedef struct
{
    unsigned char hour:5; // 5 bits are enough to represent hours from 0 to 23
}stTimerAI;

typedef struct 
{
    signed int entropy;
    unsigned char beta; 
    unsigned char dayIndex;
    unsigned char markovState;
}stValues;

typedef struct 
{
    enPersonaAI persona[_ENDDAY];
    enPersonaAI predictedPersona;

    unsigned char isEnabled:1;
    unsigned char isActivated:1;
    
    stTimerAI timer;

    stDoorInfo door;
    stFeatures features;
    stValues values;

    unsigned char dayIndex[NUMBER_OF_PERSONAS*NUMBER_OF_PERSONAS];
}stRefrigeratorAI;


#endif //AI_COOL_V1_5


#endif //_Refrigerator_AI_COOL_H
