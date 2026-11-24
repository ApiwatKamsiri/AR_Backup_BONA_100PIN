#include "rf_doors_closed.h"

void rfDoorsClosedInit(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    rfP->Values.doorsClosed = true;
    rfP->Values.doorsClosedState = false;

    if (rfP->Values.doorsClosedBlocked)
    {
        // rfP->Values.doorsClosedTimer = 0;     ///////////  DC revized algorithm
        // rfP->Values.doorsClosedBlockTimer = 0;   ///////////  DC revized algorithm
    }
    else
    {
        rfP->Values.doorsClosedTimer =
            rfC->Parameters.doorsClosedTime;
        rfP->Values.doorsClosedBlockTimer =
            rfC->Parameters.doorsClosedBlockTime;
    }

    rfP->Values.doorsOpenedCounter = 0;
}

void rfDoorsClosed1MinTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    if (rfP->Values.doorsClosedTimer)
        rfP->Values.doorsClosedTimer--;
    // if( rfP->Values.doorsClosedBlockTimer )
    // rfP->Values.doorsClosedBlockTimer--;  ///////////  DC revized algorithm
    if (rfP->Values.doorsClosedState)
        rfP->Values.doorsClosedStateStayTimer++;
}

void rfDoorsClosedTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    bool doorsClosed = true;

    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        if (rfC->Cabin[i].type != _CabinType_NONE)
        {
            if (rfP->Cabin[i].values.doorOpenedTimer)
                doorsClosed = false;
        }
    }

    if (rfP->Values.doorsClosedStateStayTimer >= rfC->Parameters.maxDoorsClosedStateStayTime)
    {
        rfP->Values.doorsClosedState = false;
        rfP->Values.doorsClosedTimer =
            rfC->Parameters.doorsClosedTime;
        rfP->Values.doorsOpenedCounter = 0;
        rfP->Values.doorsClosedStateStayTimer = 0;
    }

    if (rfP->Values.errors.sensorErrorsNumber||
        rfP->Sets.user.functions.Sabbath)
    {
        rfP->Values.doorsClosedState = false;
        rfP->Values.doorsClosedTimer =
            rfC->Parameters.doorsClosedTime;
        rfP->Values.doorsOpenedCounter = 0;
    }

    if (!doorsClosed)
    {
        if (rfP->Values.doorsClosed)
        {
            if (rfP->Values.doorsOpenedCounter)
                rfP->Values.doorsOpenedCounter--;
        }

        if (!rfP->Values.doorsOpenedCounter)
            rfP->Values.doorsClosedState = false;

        if (!rfP->Values.doorsClosedState)
        {
            rfP->Values.doorsClosedTimer =
                rfC->Parameters.doorsClosedTime;
            rfP->Values.doorsClosedStateStayTimer = 0;
        }
    }

    rfP->Values.doorsClosed = doorsClosed;

    if (rfP->Values.doorsClosedState)
    {
        rfP->Values.doorsClosedBlockTimer =
            rfC->Parameters.doorsClosedBlockTime;
    }
    else
    {
        if (!rfP->Values.doorsClosedBlockTimer)
        {
            ;
        }

        if (!rfP->Values.doorsClosedBlocked &&
            !rfP->Values.doorsClosedTimer &&
            rfC->Sets.configs.functions.DoorsClosed)
        {
            rfP->Values.doorsClosedState = true;
            rfP->Values.doorsOpenedCounter =
                rfC->Parameters.doorsOpenedCount;
        }
    }

    for (unsigned char i = 0; i < _NumOfCabin; i++)
        rfP->Cabin[i].values.doorsClosedState =
            rfP->Values.doorsClosedState;

    rfP->WaterSource.values.doorsClosedState =
        rfP->Values.doorsClosedState;

    rfP->E2.doorsClosedBlocked =
        rfP->Values.doorsClosedBlocked;
}
