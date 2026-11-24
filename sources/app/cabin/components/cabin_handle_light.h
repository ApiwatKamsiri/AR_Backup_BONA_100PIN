#ifndef _CabinALL_HandleLight_H
#define _CabinALL_HandleLight_H

#include "../cabin.h"

#define HANDLE_LIGHT_STEP 5
#define HANDLE_LIGHT_CLOSE_TIME 60*2 //60sec

void cabinHandleLightAnimationTask(tsCabinProcParams *params);
void cabinHandleLightT1sec(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinHandleLightTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void scaleColorToPWM(RGBColor color, float intensity, unsigned char *r_pwm, unsigned char *g_pwm, unsigned char *b_pwm);



#endif