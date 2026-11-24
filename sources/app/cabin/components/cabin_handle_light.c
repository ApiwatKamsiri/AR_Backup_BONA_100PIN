#include "cabin_handle_light.h"

#define STEP 2  // Transition speed (1-5 between recommend)

#define UPDATE_CHANNEL(ch_current, ch_target) \
        if (ch_current < ch_target) \
            ch_current = (ch_target - ch_current > STEP) ? (ch_current + STEP) : ch_target; \
        else if (ch_current > ch_target) \
            ch_current = (ch_current - ch_target > STEP) ? (ch_current - STEP) : ch_target;
            
//const RGBColor colorPalette[12] = {
const RGBColor colorPalette[15] = {     
    {0, 0, 0},         // Kapali (Black)
    {255, 255, 255},   // Beyaz (White)
    {160, 160, 30},   // Gün Isigi (Lemon Chiffon - hafif sarimsi beyaz)
    {0, 5, 255},       // Koyu Mavi (Dark Blue)
    {0, 255, 255},     // Açik Mavi (Light Blue)
    {12, 180, 12},       // Koyu Yesil (Dark Green)
    {40, 180, 0},   // Açik Yesil (Light Green)
    {180, 0, 0},       // Kirmizi (Red)
    {120, 10, 30},   // Pembe (Pink)
    {128, 10, 128},     // Mor (Purple)
    {150, 150, 4},     // Sari (Yellow)
    {200, 35, 0},     // Turuncu (Orange)
    {0, 255, 0},       // (Green)               //APW add for CT check (Red/Green/Blue wire check)
    {0, 0, 255},        // (Blue)                //APW add for CT check (Red/Green/Blue wire check)
    {255, 0, 0}        // (Red)                 //APW add for CT check (Red/Green/Blue wire check)
};

unsigned char handleLightState = 0;
unsigned char handleLightStatePrev = 0;

unsigned char handleLightCloseTimer = HANDLE_LIGHT_CLOSE_TIME;

void cabinHandleLightT1sec(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);
    UNUSED(cabinP);
     if(params->rf->Values.doorsClosed)
       if(handleLightCloseTimer)
         handleLightCloseTimer--;
}

void scaleColorToPWM(RGBColor color, float intensity, unsigned char *r_pwm, unsigned char *g_pwm, unsigned char *b_pwm)
{
    *r_pwm = (unsigned char)((color.r / 255.0f) * intensity * 100.0f);
    *g_pwm = (unsigned char)((color.g / 255.0f) * intensity * 100.0f);
    *b_pwm = (unsigned char)((color.b / 255.0f) * intensity * 100.0f);
}

void cabinHandleLightTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{  

    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);
    UNUSED(cabinP);

    if(handleLightStatePrev != params->rf->Values.handleLightState)
    {
        handleLightStatePrev = params->rf->Values.handleLightState;
        handleLightCloseTimer = params->rf->Values.handleLightState ? HANDLE_LIGHT_CLOSE_TIME : 0;
    }

    if((params->cabin->components.humanSensor.value != _SwitchValue_CLOSE) || (!params->rf->Values.doorsClosed) || 
        ((params->rf->ui.states.normal.state != _UI_NormalState_DisplayOff) && (params->rf->ui.states.commercial.status == _UI_Commercial_None)) ||
            ((params->rf->ui.states.commercial.state != _UI_CommercialState_DisplayOff) && (params->rf->ui.states.commercial.status != _UI_Commercial_None)))
    {
        handleLightCloseTimer = HANDLE_LIGHT_CLOSE_TIME;
    }

    float intensity = 0.0f;

    if (refrigerator_R.ui.dspPacket.funcs.ledApplianceOff)
    {
        intensity = 0.0f;
    }
    else if (refrigerator_R.workMode.values.workMode == _WM_Distributer)
    {
        intensity = handleLightCloseTimer ? 1.0f : 0.0f;
    }
    else if (refrigerator_R.workMode.values.workMode == _WM_CommercialTest)
    {
        if (params->rf->Values.handleLightState)
        {
            intensity = handleLightCloseTimer ? 0.5f : 0.5f;
        }
        else
        {
            intensity = handleLightCloseTimer ? 0.5f : 0.0f;
        }
    }
    else
    {
        if (params->rf->Values.handleLightState)
        {
            intensity = handleLightCloseTimer ? 1.0f : 0.2f;
        }
        else
        {
            intensity = handleLightCloseTimer ? 1.0f : 0.0f;
        }
    }

    RGBColor baseColor = colorPalette[cabinP->values.selectedColorIndex];

    unsigned char r_pwm = 0, g_pwm = 0, b_pwm = 0;
    scaleColorToPWM(baseColor, intensity, &r_pwm, &g_pwm, &b_pwm);

    cabinP->components.handleLight.R_value_target = r_pwm;
    cabinP->components.handleLight.G_value_target = g_pwm;
    cabinP->components.handleLight.B_value_target = b_pwm;
}

void cabinHandleLightAnimationTask(tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinP);
    
    
    UPDATE_CHANNEL(cabinP->components.handleLight.R_value, cabinP->components.handleLight.R_value_target);
    UPDATE_CHANNEL(cabinP->components.handleLight.G_value, cabinP->components.handleLight.G_value_target);
    UPDATE_CHANNEL(cabinP->components.handleLight.B_value, cabinP->components.handleLight.B_value_target);
    
}