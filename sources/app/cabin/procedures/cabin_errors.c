#include "cabin_errors.h"

static void checkHighTempErrConditions(tsCabinProcConsts const *consts, tsCabinProcParams *params);
    
void cabinCheckHighTempErr(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;

    if ((cabinC->type != _CabinType_NONE) &&
        (cabinC->type == _CabinType_FRZ) &&
        (cabinC->components.airTempSensor.type))
    {

        checkHighTempErrConditions(consts, params);

        if (cabinP->values.HighTemErr.activationTimeResetF)
        {
            cabinP->values.HighTemErr.activationTimeResetF = 0;
            cabinP->values.HighTemErr.activationTimer = 0;
            cabinP->values.HighTemErr.defrostBlocked = 0;
        }

        switch (cabinP->values.HighTemErr.highTempStatus)
        {
            case _idle:
                cabinP->values.HighTemErr.highTempStatus = _checkalarmCond;
                if (!cabinP->values.HighTemErr.isactive)
                    cabinP->values.HighTemErr.maxFRZTempInCext = cabinResetMaxTempInCext(consts, params);
            break;

            case _checkalarmCond:
                if ((cabinP->components.airTempSensor.valueInCExt > cabinC->parameters.HighTempError.HighTempErrCutIN) && (cabinP->values.HighTemErr.FirstCutoutDoneF))
                {
                    cabinP->values.HighTemErr.highTempStatus = _waitBeforeActivation;
                }
                cabinP->values.HighTemErr.activationTimeResetF = 1;

                if (!cabinP->values.HighTemErr.isactive)
                    cabinP->values.HighTemErr.maxFRZTempInCext = cabinResetMaxTempInCext(consts, params);
            break;

            case _waitBeforeActivation:
                if (cabinP->components.airTempSensor.valueInCExt < cabinC->parameters.HighTempError.HighTempErrCutOUT)
                    cabinP->values.HighTemErr.highTempStatus = _checkalarmCond;

                if (cabinP->values.HighTemErr.isactive)
                    cabinP->values.HighTemErr.highTempStatus = _alarmIsActive;
            break;

            case _alarmIsActive:
                if (!cabinP->values.HighTemErr.isactive)
                {
                    if (cabinP->components.airTempSensor.valueInCExt < cabinC->parameters.HighTempError.HighTempErrCutOUT)
                    {
                        cabinP->values.HighTemErr.activationTimeResetF = 1;
                        cabinP->values.HighTemErr.highTempStatus = _idle;
                    }
                    else
                    {
                        //cabinP->values.HighTemErr.isactive = 1;
                        if (!cabinP->values.HighTemErr.delayTime)
                            cabinP->values.HighTemErr.isactive = 1;
                        //else
                        //cabinP->values.HighTemErr.delayTime--;
                    }
                }
                /*else
                {
                    if (!cabinC->parameters.permannentHighTempError)
                    {
                        if (cabinP->components.airTempSensor.valueInCExt < cabinC->parameters.HighTempError.HighTempErrCutOUT)
                        {
                            cabinP->values.HighTemErr.activationTimeResetF = 1;
                            cabinP->values.HighTemErr.highTempStatus = _idle;
                            cabinP->values.HighTemErr.isactive = 0;
                        }
                    }
                }*/
            break;

            default:
                cabinP->values.HighTemErr.highTempStatus = _idle;
            break;
        }
    }
}

void checkHighTempErrConditions(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;
	UNUSED(cabinC);

    if ((cabinP->components.airTempSensor.valueInCExt > -400) &&
        (cabinP->values.HighTemErr.algorithmActivationTime >= 4320/* cabinC->parameters.HighTempError.HighTmp_Algorithm_Activation */))
    {
        //(cabinP->components.airTempSensor.value <= cabinSetsDatasTable_F[17].coolCutOutTemp))
        cabinP->values.HighTemErr.FirstCutoutDoneF = 1;
    }

    if (cabinP->values.HighTemErr.FirstCutoutDoneF)
    {
        if (cabinP->values.HighTemErr.maxFRZTempInCext < cabinP->components.airTempSensor.valueInCExt)
        {
            cabinP->values.HighTemErr.maxFRZTempInCext = cabinP->components.airTempSensor.valueInCExt;
        }
    }

    if (cabinP->values.HighTemErr.maxFRZTempInCext > 250)
        cabinP->values.HighTemErr.maxFRZTempInCext = 250;
}

void cabinCheckHighTempErr1MinTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;

    if ((cabinC->type != _CabinType_NONE) &&
        (cabinC->type == _CabinType_FRZ) &&
        (cabinC->components.airTempSensor.type))
    {

        if (cabinP->values.HighTemErr.algorithmActivationTime <= (((/* cabinC->parameters.HighTempError.HighTmp_Algorithm_Activation */4320 / 60) + 1) * 60))
            cabinP->values.HighTemErr.algorithmActivationTime++;

        if (cabinP->values.HighTemErr.PowerUpCond)
        {
            if (cabinP->values.HighTemErr.erasePowerUpTime < 7)
                cabinP->values.HighTemErr.erasePowerUpTime++;
            else
                cabinP->values.HighTemErr.PowerUpCond = 0;
        }

        if (cabinP->values.HighTemErr.highTempStatus == _waitBeforeActivation)
        {
            if (cabinP->values.HighTemErr.PowerUpCond) //Elektrik kesintisi
            {
                if (cabinP->values.HighTemErr.activationTimer < cabinC->parameters.HighTempError.HighTmp_Time_PowerUp)
                {
                    cabinP->values.HighTemErr.activationTimer++;
                    cabinP->values.HighTemErr.isactive = 0;
                }
                else
                {
                    cabinP->values.HighTemErr.isactive = 1;
                }
            }
            else if ((cabinP->states.Work == _Work_Defrost) ||
                     (cabinP->states.Work == _Work_Drainage))
            {
                cabinP->values.HighTemErr.defrostBlocked = 1;
            }
            else if (cabinP->values.HighTemErr.defrostBlocked == 1)
            {
                if (cabinP->values.HighTemErr.activationTimer < cabinC->parameters.HighTempError.HighTmp_Time_Defrost)
                    cabinP->values.HighTemErr.activationTimer++;
                else
                {
                    cabinP->values.HighTemErr.defrostBlocked = 0;
                    cabinP->values.HighTemErr.isactive = 1;
                }
            }
            else
            {
                if (cabinP->values.HighTemErr.activationTimer < cabinC->parameters.HighTempError.HighTmp_Time_Normal)
                    cabinP->values.HighTemErr.activationTimer++;
                else
                    cabinP->values.HighTemErr.isactive = 1;
            }
        }
    }

    cabinP->E2.highTempStatus = cabinP->values.HighTemErr.highTempStatus;
    cabinP->E2.HighTempIsactive = cabinP->values.HighTemErr.isactive;
    cabinP->E2.FirstCutoutDoneF = cabinP->values.HighTemErr.FirstCutoutDoneF;
    if (cabinP->values.HighTemErr.algorithmActivationTime % 60 == 0) //Dakikada bir e2'a yazmaz. Saatte bir girer.
    {
        cabinP->E2.AlgorithmActivationTime = cabinP->values.HighTemErr.algorithmActivationTime;
    }
    cabinP->E2.maxFRZTempInCext = cabinP->values.HighTemErr.maxFRZTempInCext;
}

signed short cabinResetMaxTempInCext(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;
	UNUSED(cabinP);

    return (cabinC->parameters.HighTempError.HighTempErrCutOUT - 100);
}

void cabinCheckDefrostHeaterErr(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;

    if ((cabinC->type != _CabinType_NONE) &&
        (cabinP->sets.user.Mode == _CabinMode_FRZ))
    {
        if (cabinP->values.errDefrostHeaterCounter >= 4)
            cabinP->values.errDefrostHeater = true;
        else
            cabinP->values.errDefrostHeater = false;
    }
    else
    {
        cabinP->values.errDefrostHeater = false;
        cabinClearDefrostHeaterErrCounter(consts, params);
    }
}

void cabinIncDefrostHeaterErrCounter(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;
	UNUSED(cabinC);

    if (cabinP->values.errDefrostHeaterCounter < 0xFF)
        cabinP->values.errDefrostHeaterCounter++;

    cabinP->E2.errDefrostHeaterCounter = cabinP->values.errDefrostHeaterCounter;
}

void cabinClearDefrostHeaterErrCounter(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;
	UNUSED(cabinC);

    cabinP->values.errDefrostHeaterCounter = 0;
    cabinP->E2.errDefrostHeaterCounter = cabinP->values.errDefrostHeaterCounter;
}

void cabinSensorsErr(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;
	UNUSED(cabinC);
	cabinP->values.errSensors = params->rf->Values.errors.sensorErrorsNumber;
}
