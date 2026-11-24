#include "rf_ai_cool.h"


#if defined(AI_COOL_V1_5)

#include "../refrigerator.h"
#include "app/mw/connectivity/remote_control.h"
#include "app/cooler/components/cooler_valve.h"
#include <math.h>
#include <stdint.h>

static void extractFeatures(stRefrigerator_Ram *rfP);
static void findEuclideanDistancesAndPersona(stRefrigerator_Ram *rfP);
static bool isDoorStateChangedtoOpen(stRefrigerator_Ram *rfP);
static void updateCustomerTransitionMatrix(stRefrigerator_Ram *rfP);
static void calculateBetaValue(stRefrigerator_Ram *rfP);
static void calculateEntropy(stRefrigerator_Ram *rfP);
static void calculateDynamicMarkovProbability(stRefrigerator_Ram *rfP);
static void normalizeTransitionMatrix(stRefrigerator_Ram *rfP);

void rfPRamToE2(stRefrigerator_Ram *rfP);
void rfPE2ToRam(stRefrigerator_Ram *rfP);


static unsigned short distanceArray[NUMBER_OF_PERSONAS][NUMBER_OF_FEATURES] = {
    {2,   0,   7,  31,  6,   63}, //_PERSONA_OCCASIONAL_SNACKER
    {13,  1,  24,  89, 26,  311}, //_PERSONA_BUSY_PROFESSIONAL
    {26,  6,  43, 170, 45,  631}, //_PERSONA_BALANCED_HOUSEHOLD
    {76, 19, 170, 766, 69, 1828}  //_PERSONA_FAMILY_MATRIARCH_CHEF
};

static unsigned char transitionMatrixStart[NUMBER_OF_PERSONAS*NUMBER_OF_PERSONAS][NUMBER_OF_PERSONAS] = {
    {66,26,6,0},
    {30,53,16,0},
    {16,45,36,3},
    {13,36,42,8},

    {43,47,10,0},
    {14,63,23,0},
    {6,46,47,2},
    {5,37,47,10},

    {35,46,18,1},
    {8,49,41,1},
    {2,26,67,4},
    {0,9,70,20},
    
    {63,32,5,0},
    {10,40,47,3},
    {1,14,71,13},
    {1,5,62,32}
};

static unsigned char transitionMatrixCustomer[NUMBER_OF_PERSONAS*NUMBER_OF_PERSONAS][NUMBER_OF_PERSONAS] = {
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},

    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},

    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},

    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
};

static const unsigned char aiCompSpeeds[NUMBER_OF_PERSONAS][NUMBER_OF_AMBIENTS] = {
    {AI_AMB0_OCCASIONAL_SNACKER_COMP_SPEED, AI_AMB1_OCCASIONAL_SNACKER_COMP_SPEED, AI_AMB2_OCCASIONAL_SNACKER_COMP_SPEED, AI_AMB3_OCCASIONAL_SNACKER_COMP_SPEED, AI_AMB4_OCCASIONAL_SNACKER_COMP_SPEED},
    {AI_AMB0_BUSY_PROFESSIONAL_COMP_SPEED, AI_AMB1_BUSY_PROFESSIONAL_COMP_SPEED, AI_AMB2_BUSY_PROFESSIONAL_COMP_SPEED, AI_AMB3_BUSY_PROFESSIONAL_COMP_SPEED, AI_AMB4_BUSY_PROFESSIONAL_COMP_SPEED},
    {AI_AMB0_BALANCED_HOUSEHOLD_COMP_SPEED, AI_AMB1_BALANCED_HOUSEHOLD_COMP_SPEED, AI_AMB2_BALANCED_HOUSEHOLD_COMP_SPEED, AI_AMB3_BALANCED_HOUSEHOLD_COMP_SPEED, AI_AMB4_BALANCED_HOUSEHOLD_COMP_SPEED},
    {AI_AMB0_FAMILY_MATRIARCH_CHEF_COMP_SPEED, AI_AMB1_FAMILY_MATRIARCH_CHEF_COMP_SPEED, AI_AMB2_FAMILY_MATRIARCH_CHEF_COMP_SPEED, AI_AMB3_FAMILY_MATRIARCH_CHEF_COMP_SPEED, AI_AMB4_FAMILY_MATRIARCH_CHEF_COMP_SPEED}
};

static const signed short aiSetOffSets[NUMBER_OF_PERSONAS][AI_CABIN_SIZE] = {
    {FF_OCCASIONAL_SNACKER_OFFSET, FRZ_OCCASIONAL_SNACKER_OFFSET},
    {FF_BUSY_PROFESSIONAL_OFFSET, FRZ_BUSY_PROFESSIONAL_OFFSET},
    {FF_BALANCED_HOUSEHOLD_OFFSET, FRZ_BALANCED_HOUSEHOLD_OFFSET},
    {FF_FAMILY_MATRIARCH_CHEF_OFFSET, FRZ_FAMILY_MATRIARCH_CHEF_OFFSET}
};

static const unsigned short aiFrzDefrostOffSets[NUMBER_OF_PERSONAS] = {
    FRZ_DEFROST_OCCASIONAL_SNACKER_OFFSET,
    FRZ_DEFROST_BUSY_PROFESSIONAL_OFFSET,
    FRZ_DEFROST_BALANCED_HOUSEHOLD_OFFSET,
    FRZ_DEFROST_FAMILY_MATRIARCH_CHEF_OFFSET
};

void rfPReset(stRefrigerator_Ram *rfP)
{
    rfP->AI.timer.hour = 0;
    rfP->AI.door.openCount = 0;
    for (unsigned char i = 0; i < NUMBER_OF_AI_ANALYZE_HOURS; i++)
    {
        rfP->AI.door.openTime[i] = 0;
    }
}

void RefrigeratorAIInit(stRefrigerator_Ram *rfP)
{
    rfP->AI.isEnabled = 0;
    rfP->AI.isActivated = 0;

    rfP->AI.persona[_BEFORE_YESTERDAY] = _PERSONA_BALANCED_HOUSEHOLD;
    rfP->AI.persona[_YESTERDAY] = _PERSONA_BALANCED_HOUSEHOLD;
    rfP->AI.persona[_TODAY] = _PERSONA_BALANCED_HOUSEHOLD;

    rfP->AI.predictedPersona = _PERSONA_BALANCED_HOUSEHOLD;

    rfP->AI.features.meanDoorOpenTime = 0;
    rfP->AI.features.medianDoorOpenTime = 0;
    rfP->AI.features.stdDevDoorOpenTime = 0;
    rfP->AI.features.maximumDoorOpenTime = 0;
    rfP->AI.features.totalDoorOpenTime = 0;
    rfP->AI.features.totalDoorOpenCount = 0;

    rfPReset(rfP);

    rfPE2ToRam(rfP);
}

void RefrigeratorAIT1sec(stRefrigerator_Ram *rfP)
{
    if (rfP->Values.doorsClosed == 0) //doorOpen
        if (rfP->AI.door.openTime[rfP->AI.timer.hour] < 3600) //prevent wrong data 1 hour max 3600 sec
            rfP->AI.door.openTime[rfP->AI.timer.hour]++;

    if (isDoorStateChangedtoOpen(rfP))
        if (rfP->AI.door.openCount < UINT8_MAX) //prevent overflow
            rfP->AI.door.openCount++;

    // Check if the door is always open for two consecutive hours and set the flag accordingly
    /*for(unsigned char i = 1; i < NUMBER_OF_AI_ANALYZE_HOURS; i++)
    {
        if (rfP->AI.door.openTime[i] >= 3450 && rfP->AI.door.openTime[i - 1] >= 3450)
            rfP->AI.door.alwaysOpen = 1;
        else
            rfP->AI.door.alwaysOpen = 0;
    }*/

}

void RefrigeratorAIT1hour(stRefrigerator_Ram *rfP)
{
    if (rfP->AI.timer.hour < NUMBER_OF_AI_ANALYZE_HOURS - 1)
    {
        rfP->AI.timer.hour++;
    }
    else
    { // 1 day completed

        normalizeTransitionMatrix(rfP);

        extractFeatures(rfP);
        findEuclideanDistancesAndPersona(rfP);

        unsigned char index = rfP->AI.persona[_BEFORE_YESTERDAY] * NUMBER_OF_PERSONAS + rfP->AI.persona[_YESTERDAY];
        if (rfP->AI.dayIndex[index] < UINT8_MAX) //prevent overflow
          rfP->AI.dayIndex[rfP->AI.persona[_BEFORE_YESTERDAY] * NUMBER_OF_PERSONAS + rfP->AI.persona[_YESTERDAY]]++;

        rfP->AI.values.dayIndex = rfP->AI.dayIndex[rfP->AI.persona[_BEFORE_YESTERDAY] * NUMBER_OF_PERSONAS + rfP->AI.persona[_YESTERDAY]];

        rfP->AI.values.markovState = rfP->AI.persona[_BEFORE_YESTERDAY] * NUMBER_OF_PERSONAS + rfP->AI.persona[_YESTERDAY];

        updateCustomerTransitionMatrix(rfP);

        calculateEntropy(rfP);
        calculateBetaValue(rfP);
        calculateDynamicMarkovProbability(rfP);

        rfPRamToE2(rfP);

        rfPReset(rfP);

        rfP->AI.timer.hour = 0;
    }

}

/**
 * @brief Calculates the total door open time for the refrigerator AI.
 *
 * This function computes the total door open time by summing the door open time
 * by the predefined number of hours. The result is stored in the 
 * totalDoorOpenTime feature of the rfP structure.
 *
 * @param rfP Pointer to the strfP structure containing the features.
 */
static void calculateTotalDoorOpenTime(stRefrigerator_Ram *rfP)
{
    unsigned int totalDoorOpenTime = 0;

    for (unsigned char i = 0; i < NUMBER_OF_AI_ANALYZE_HOURS; i++)
    {
        if (totalDoorOpenTime < UINT16_MAX - rfP->AI.door.openTime[i]) //prevent overflow
            totalDoorOpenTime += rfP->AI.door.openTime[i];
        else
            totalDoorOpenTime = UINT16_MAX; // Assign maximum value to prevent overflow

    }

    rfP->AI.features.totalDoorOpenTime = totalDoorOpenTime;
}

/**
 * @brief Calculates the mean door open time for the refrigerator AI.
 *
 * This function computes the mean door open time by dividing the total door open time
 * by the predefined number of hours to analyze. The result is stored in the 
 * meanDoorOpenTime feature of the rfP structure.
 *
 * @param rfP Pointer to the strfP structure containing the features.
 */
static void calculateMeanDoorOpenTime(stRefrigerator_Ram *rfP)
{
    rfP->AI.features.meanDoorOpenTime = rfP->AI.features.totalDoorOpenTime / NUMBER_OF_AI_ANALYZE_HOURS;
}

/**
 * @brief Calculates the median door open time for the refrigerator AI.
 *
 * This function sorts the door open times recorded over a specified number of hours
 * and calculates the median value. The median is then stored in the rfP
 * structure's features.
 *
 * @param rfP Pointer to the strfP structure containing door open times.
 */
static void calculateMedianDoorOpenTime(stRefrigerator_Ram *rfP)
{
    unsigned int sortedOpenTime[NUMBER_OF_AI_ANALYZE_HOURS];
    unsigned int temp;

    for (unsigned char i = 0; i < NUMBER_OF_AI_ANALYZE_HOURS; i++)
        sortedOpenTime[i] = rfP->AI.door.openTime[i];

    for (unsigned char i = 0; i < NUMBER_OF_AI_ANALYZE_HOURS; i++)
    {
        for (unsigned char j = i + 1; j < NUMBER_OF_AI_ANALYZE_HOURS; j++)
        {
            if (sortedOpenTime[i] > sortedOpenTime[j])
            {
                temp = sortedOpenTime[i];
                sortedOpenTime[i] = sortedOpenTime[j];
                sortedOpenTime[j] = temp;
            }
        }
    }

    if (NUMBER_OF_AI_ANALYZE_HOURS % 2 == 0)
        rfP->AI.features.medianDoorOpenTime = (sortedOpenTime[NUMBER_OF_AI_ANALYZE_HOURS / 2] + sortedOpenTime[(NUMBER_OF_AI_ANALYZE_HOURS / 2) - 1]) / 2;
    else
        rfP->AI.features.medianDoorOpenTime = sortedOpenTime[NUMBER_OF_AI_ANALYZE_HOURS / 2];
}

/**
 * @brief Calculates the standard deviation of the door open time for the refrigerator AI.
 *
 * This function computes the standard deviation of the door open time based on the mean door open time
 * and the recorded door open times over a specified number of hours. The result is stored in the 
 * stdDevDoorOpenTime field of the rfP structure.
 *
 * @param rfP Pointer to the strfP structure containing the door open times and features.
 */
static void calculateStdDevDoorOpenTime(stRefrigerator_Ram *rfP)
{
    unsigned char meanDoorOpenTime = rfP->AI.features.meanDoorOpenTime;
    unsigned int totalDoorOpenTime = 0;

    for (unsigned char i = 0; i < NUMBER_OF_AI_ANALYZE_HOURS; i++)
        totalDoorOpenTime += (rfP->AI.door.openTime[i] - (unsigned int)meanDoorOpenTime) * (rfP->AI.door.openTime[i] - (unsigned int)meanDoorOpenTime);

    rfP->AI.features.stdDevDoorOpenTime = (unsigned char)sqrt(totalDoorOpenTime / NUMBER_OF_AI_ANALYZE_HOURS);
}

/**
 * @brief Calculates the maximum door open time for the refrigerator AI.
 *
 * This function iterates through the door open times recorded in the 
 * rfP structure and determines the maximum door open time.
 * The result is then stored in the features.maximumDoorOpenTime field 
 * of the rfP structure.
 *
 * @param rfP Pointer to the strfP structure containing 
 *                       the door open times and features.
 */
static void calculateMaximumDoorOpenTime(stRefrigerator_Ram *rfP)
{
    unsigned int maximumDoorOpenTime = 0;

    for (unsigned char i = 0; i < NUMBER_OF_AI_ANALYZE_HOURS; i++)
    {
        if (rfP->AI.door.openTime[i] > maximumDoorOpenTime)
            maximumDoorOpenTime = rfP->AI.door.openTime[i];
    }

    rfP->AI.features.maximumDoorOpenTime = maximumDoorOpenTime;
}

/**
 * @brief Calculates and updates the total door open count for the refrigerator AI.
 *
 * This function takes a pointer to a strfP structure and updates its
 * totalDoorOpenCount feature by assigning it the value of the door's openCount.
 *
 * @param rfP Pointer to the strfP structure.
 */
static void calculateTotalDoorOpenCount(stRefrigerator_Ram *rfP)
{
    rfP->AI.features.totalDoorOpenCount = rfP->AI.door.openCount;
}

/**
 * @brief Extracts various features related to the refrigerator's door open times and counts.
 *
 * This function calculates and extracts several statistical features from the refrigerator's
 * door open times and counts, including total door open time, mean door open time, median door
 * open time, standard deviation of door open time, maximum door open time, and total door open count.
 *
 * @param rfP Pointer to the strfP structure containing the refrigerator data.
 */
static void extractFeatures(stRefrigerator_Ram *rfP)
{
    calculateTotalDoorOpenTime(rfP);
    calculateMeanDoorOpenTime(rfP);
    calculateMedianDoorOpenTime(rfP);
    calculateStdDevDoorOpenTime(rfP);
    calculateMaximumDoorOpenTime(rfP);
    calculateTotalDoorOpenCount(rfP);
}

/**
 * @brief Calculates the Euclidean distances between the current refrigerator AI features and predefined personas,
 *        and assigns the closest persona to the refrigerator AI.
 *
 * This function computes the Euclidean distance between the current feature set of the refrigerator AI and each
 * predefined persona's feature set. It then determines the persona with the minimum distance and updates the
 * refrigerator AI's persona history accordingly.
 *
 * @param rfP Pointer to the strfP structure containing the current features and persona history.
 */
static void findEuclideanDistancesAndPersona(stRefrigerator_Ram *rfP)
{
    unsigned int distance = 0;
    unsigned int minDistance = 0xFFFF;
    unsigned char minDistanceIndex = 0;

    unsigned int featureArray[NUMBER_OF_FEATURES] = {
        rfP->AI.features.meanDoorOpenTime,    //MEAN
        rfP->AI.features.medianDoorOpenTime,  //MEDIAN
        rfP->AI.features.stdDevDoorOpenTime,  //STD
        rfP->AI.features.maximumDoorOpenTime, //MAX
        rfP->AI.features.totalDoorOpenCount,  //NUM_EVENTS
        rfP->AI.features.totalDoorOpenTime    //TOTAL_DURATION
    };

    for (unsigned char i = 0; i < NUMBER_OF_PERSONAS; i++)
    {
        distance = 0;
        for (unsigned char j = 0; j < NUMBER_OF_FEATURES; j++)
            distance +=  (distanceArray[i][j] - featureArray[j]) * (distanceArray[i][j] - featureArray[j]);

        distance = (unsigned int)sqrt(distance);

        if (distance < minDistance)
        {
            minDistance = distance;
            minDistanceIndex = i;
        }
    }

    rfP->AI.persona[_BEFORE_YESTERDAY] = rfP->AI.persona[_YESTERDAY];
    rfP->AI.persona[_YESTERDAY] = rfP->AI.persona[_TODAY];
    rfP->AI.persona[_TODAY] = (enPersonaAI)minDistanceIndex;
}

/**
 * @brief Checks if the door state has changed to open.
 *
 * This function compares the current door state with the previous door state.
 * If the door state has changed from closed to open, it returns true.
 *
 * @param refrigerator_RPtr Pointer to the refrigerator RAM structure containing the current door state.
 * @return true if the door state has changed to open, false otherwise.
 */
static bool isDoorStateChangedtoOpen(stRefrigerator_Ram *rfP)
{
    static unsigned char prevDoorState = 0;
    bool isStateChanged = 0;

    if (prevDoorState != rfP->Values.doorsClosed)
    {
        if (rfP->Values.doorsClosed == 0)
            isStateChanged = 1;

        prevDoorState = rfP->Values.doorsClosed;
    }

    return isStateChanged;
}

/**
 * @brief Calculates the beta value for the refrigerator AI.
 *
 * This function calculates the beta value based on the day index and entropy
 * values of the refrigerator AI. If the day index is less than or equal to 20,
 * the beta value is calculated by shifting the day index left by 1 (equivalent
 * to multiplying by 2). If the day index is greater than 20, the beta value is
 * calculated as 100 minus the entropy value.
 *
 * @param rfP Pointer to the strfP structure containing
 *                       the values used for calculation.
 */
static void calculateBetaValue(stRefrigerator_Ram *rfP)
{
    unsigned char betaValue = 0;

    if(rfP->AI.values.dayIndex <= 20)
        betaValue = (rfP->AI.values.dayIndex << 1); // normally divide by 100 but for decimal point operation we did not
    else
        betaValue = (100 - rfP->AI.values.entropy);

    rfP->AI.values.beta = betaValue;
}

static void calculateEntropy(stRefrigerator_Ram *rfP)
{
    float entropy = 0;
    float toIthStateProbablity = 0;
    //unsigned char markovState = rfP->AI.persona[_YESTERDAY] * NUMBER_OF_PERSONAS + rfP->AI.persona[_TODAY];
    unsigned char markovState = rfP->AI.values.markovState;
    unsigned int totalObservedDataForState = 0;

    for(unsigned char i = 0; i < NUMBER_OF_PERSONAS; i++)
        totalObservedDataForState += transitionMatrixCustomer[markovState][i];

    for (unsigned char i = 0; i < NUMBER_OF_PERSONAS; i++)
    {
        if(transitionMatrixCustomer[markovState][i] != 0) //if all states are 0, then our answer returns 0 => incorrect bcz the stable is highly unstable bcz no info found. In a way, this statement is also incorrect bcz there is no data so we should not assign 1. Since our algo completely ignores until atleast 20 days worth of state data found, we need not concern ourselves with this more. In practice we never encounter this situation.
        {
            toIthStateProbablity = (float)transitionMatrixCustomer[markovState][i] / (float)totalObservedDataForState;
            entropy += (float)toIthStateProbablity * (float)log(toIthStateProbablity);
        }
    }

    entropy = entropy / 1.386; // log2(4) = 1.386 important ! we need to normalize the entropy value

    rfP->AI.values.entropy = -1 * (signed int)(entropy * (float)100); //to be consistent with other values used to avoid decimal point operations
}

/**
 * @brief Calculates the dynamic Markov probability for the refrigerator AI.
 *
 * This function computes the probabilities of different personas based on the 
 * Markov state and transition matrices. It updates the predicted persona in 
 * the refrigerator AI structure.
 *
 * @param rfP Pointer to the refrigerator AI structure containing 
 *                       the necessary data for calculation.
 */
static void calculateDynamicMarkovProbability(stRefrigerator_Ram *rfP)
{
    unsigned char beta = rfP->AI.values.beta;
    float probablities[NUMBER_OF_PERSONAS] = {0};
    unsigned char indexMaxProbablity = 0;
    //unsigned char markovState = rfP->AI.persona[_YESTERDAY] * NUMBER_OF_PERSONAS + rfP->AI.persona[_TODAY];
    unsigned char markovState = rfP->AI.values.markovState;
    unsigned int totalObservedDataForState = 0;

    for(unsigned char i = 0; i < NUMBER_OF_PERSONAS; i++)
        totalObservedDataForState += transitionMatrixCustomer[markovState][i];


    for(unsigned char i = 0; i < NUMBER_OF_PERSONAS; i++)
    {
        if(transitionMatrixCustomer[markovState][i] != 0)
            probablities[i] = ((float)beta * (float)100 * ((float)transitionMatrixCustomer[markovState][i]/(float)totalObservedDataForState)) + ( (float)(100-beta) * (float)transitionMatrixStart[markovState][i]);
        else
            probablities[i] = ((float)(100-beta) * (float)transitionMatrixStart[markovState][i]);
    }


   // find maximum value in probablities array
    for(unsigned char i = 0; i < NUMBER_OF_PERSONAS; i++)
        if(probablities[i] > probablities[indexMaxProbablity])
            indexMaxProbablity = i;

    rfP->AI.predictedPersona = (enPersonaAI)indexMaxProbablity;
}

/**
 * @brief Updates the customer transition matrix based on the current Markov state and ground truth persona.
 *
 * This function increments the value in the transition matrix for the given Markov state and ground truth persona,
 * ensuring that the value does not overflow the maximum value of an unsigned 8-bit integer.
 *
 * @param rfP Pointer to the strfP structure containing the current Markov state and persona information.
 */
static void updateCustomerTransitionMatrix(stRefrigerator_Ram *rfP)
{
    unsigned char markovState = rfP->AI.values.markovState;
    enPersonaAI groundTruth = rfP->AI.persona[_TODAY];

    if(groundTruth < NUMBER_OF_PERSONAS && markovState < (NUMBER_OF_PERSONAS*NUMBER_OF_PERSONAS))
        if(transitionMatrixCustomer[markovState][groundTruth] < UINT8_MAX) // Prevent overflow
            transitionMatrixCustomer[markovState][groundTruth] += 1;
}

/**
 * @brief Normalize the transition matrix for personas where dayIndex is 254.
 *
 * This function iterates through the transition matrix of the refrigerator AI and normalizes
 * the values for personas where the dayIndex is 254. It resets the total to 0, normalizes the
 * values to a percentage, and updates the dayIndex with the new total.
 *
 * @param rfP Pointer to the strfP structure containing the transition matrix and dayIndex.
 */
static void normalizeTransitionMatrix(stRefrigerator_Ram *rfP)
{
        // Normalize the transition matrix for personas where dayIndex is 254.
        // Reset total to 0, normalize values to a percentage, and update dayIndex.

        unsigned char total = 0;

        for(unsigned char i = 0; i < NUMBER_OF_PERSONAS*NUMBER_OF_PERSONAS; i++)
        {
            if(rfP->AI.dayIndex[i] == 254)
            {
                total = 0;
                for(unsigned char j = 0; j < NUMBER_OF_PERSONAS; j++)
                {
                    transitionMatrixCustomer[i][j] = (unsigned char)(((float)transitionMatrixCustomer[i][j]/(float)254)*(float)100);
                    total += transitionMatrixCustomer[i][j];
                }
                rfP->AI.dayIndex[i] = total;
            }
        }
}

/**
 * @brief Copies the transition matrix from RAM to EEPROM for the Refrigerator AI.
 *
 * This function iterates over the transition matrix of the Refrigerator AI stored in RAM
 * and copies its contents to the corresponding EEPROM storage.
 *
 * @param rfP Pointer to the Refrigerator AI structure containing the transition matrix in RAM.
 */
void rfPRamToE2(stRefrigerator_Ram *rfP)
{
        for(unsigned char i = 0; i < NUMBER_OF_PERSONAS*NUMBER_OF_PERSONAS; i++)
        {
            for(unsigned char j = 0; j < NUMBER_OF_PERSONAS; j++)
            {
                rfP->E2.AI.transitionMatrixCustomer[i][j] = transitionMatrixCustomer[i][j];
            }
        }    
}

/**
 * @brief Transfers the transition matrix data from the E2 customer AI structure to the RAM and updates the day index.
 *
 * This function copies the transition matrix data from the E2 customer AI structure to the transition matrix in RAM.
 * Additionally, it updates the day index in the provided rfP structure based on the values in the transition matrix.
 *
 * @param rfP Pointer to the strfP structure where the day index will be updated.
 */
void rfPE2ToRam(stRefrigerator_Ram *rfP)
{
        for(unsigned char i = 0; i < NUMBER_OF_PERSONAS*NUMBER_OF_PERSONAS; i++)
        {
            for(unsigned char j = 0; j < NUMBER_OF_PERSONAS; j++)
            {
                transitionMatrixCustomer[i][j] = rfP->E2.AI.transitionMatrixCustomer[i][j];
                rfP->AI.dayIndex[i] += transitionMatrixCustomer[i][j]; //recover day index information from e2customer transition matrix
            }
        }
}

/**
 * @brief Adjusts the compressor speed based on the predicted persona and other parameters.
 *
 * @param CoolerC Pointer to the cooler flash parameters.
 * @param CoolerP Pointer to the cooler RAM parameters.
 * @param rfP Pointer to the refrigerator AI structure.
 * @return The adjusted compressor speed.
 */
unsigned char RefrigeratorAIChangeCompressorSpeed(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    unsigned char compSpeed = 0;
    const stCooler_Flash *CoolerC = &rfC->Cooler;
    stCooler_Ram * CoolerP = &rfP->Cooler;

    enPersonaAI predictedPersona = rfP->AI.predictedPersona;//(enPersonaAI)WiFiVariables.AIPersonaSelect;//

    if (CoolerP->components.solenoidValve[0].positionIndex == _FF_OPEN)
        compSpeed = CoolerC->parameters.compressor.FF.speedLevel[CoolerP->parameters.ambientType][CoolerP->parameters.gasLevel - 1];
    else if (CoolerP->components.solenoidValve[0].positionIndex == _FRZ_OPEN)
        compSpeed = CoolerC->parameters.compressor.FRZ.speedLevel[CoolerP->parameters.ambientType][CoolerP->parameters.gasLevel - 1];
    else
        compSpeed = CoolerC->parameters.compressor.FF.speedLevel[CoolerP->parameters.ambientType][CoolerP->parameters.gasLevel - 1];

    if(rfP->AI.isEnabled == 1)
        if(CoolerP->parameters.compressorOnTimerPrev <= AI_COMPRESSOR_ONTIMER_THRESHOLD)
            if (predictedPersona < NUMBER_OF_PERSONAS && CoolerP->parameters.ambientType < NUMBER_OF_AMBIENTS)
                compSpeed = aiCompSpeeds[predictedPersona][CoolerP->parameters.ambientType];

    return compSpeed;
}

/**
 * @brief Decreases the cabin set temperatures if required based on the predicted persona.
 *
 * @param cabinC Pointer to the cabin flash parameters.
 * @param cabinP Pointer to the cabin RAM parameters.
 * @param rfP Pointer to the refrigerator structure.
 */
void RefrigeratorAIDecreaseCabinSetsIfRequired(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    unsigned char criticalSetIndex = 0;
    unsigned char selectedCabin = 0;
    unsigned char cabinIndex = 0;
    const stCabin_Flash *cabinC = NULL;
    stCabin_Ram *cabinP = NULL;
    enPersonaAI predictedPersona = rfP->AI.predictedPersona;//(enPersonaAI)WiFiVariables.AIPersonaSelect; //

    if(rfP->AI.isEnabled != 1)
        return;

    // Check all cabins
    for(cabinIndex = 0; cabinIndex < _NumOfCabin; cabinIndex++)
    {
        cabinC = &rfC->Cabin[cabinIndex];
        cabinP = &rfP->Cabin[cabinIndex];
        criticalSetIndex = 0;
        selectedCabin = 0;

        if(cabinC->type == _CabinType_FF)
        {
            if(cabinC->sets.indexes.FF[cabinP->sets.user.FF] < _AI_FF_CRITICAL_SET_INDEX)
            {
                criticalSetIndex = _AI_FF_CRITICAL_SET_INDEX;
                selectedCabin = FF_OFFSET;
            }
        }
        else if(cabinC->type == _CabinType_FRZ)
        {
            if(cabinC->sets.indexes.FRZ[cabinP->sets.user.FRZ] < _AI_FRZ_CRITICAL_SET_INDEX)
            {
                criticalSetIndex = _AI_FRZ_CRITICAL_SET_INDEX;
                selectedCabin = FRZ_OFFSET;
            }
        }

        if(criticalSetIndex == 0 || selectedCabin >= AI_CABIN_SIZE || predictedPersona >= NUMBER_OF_PERSONAS)
            continue;

        cabinP->sets.datas.coolCutInTemp = cabinSetsDatasTable_F[criticalSetIndex].coolCutInTemp + aiSetOffSets[predictedPersona][selectedCabin];
        cabinP->sets.datas.coolCutOutTemp = cabinSetsDatasTable_F[criticalSetIndex].coolCutOutTemp + aiSetOffSets[predictedPersona][selectedCabin];
        cabinP->sets.datas.fanCutInTemp = cabinSetsDatasTable_F[criticalSetIndex].fanCutInTemp + aiSetOffSets[predictedPersona][selectedCabin];
        cabinP->sets.datas.fanCutOutTemp = cabinSetsDatasTable_F[criticalSetIndex].fanCutOutTemp + aiSetOffSets[predictedPersona][selectedCabin];
    }
}

/**
 * @brief Adjusts the defrost intervals based on the predicted persona and other parameters.
 *
 * @param cabinC Pointer to the cabin flash parameters.
 * @param cabinP Pointer to the cabin RAM parameters.
 * @param rfP Pointer to the refrigerator structure.
 */
void RefrigeratorAIChangeDefrostIntervals(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    unsigned char cabinIndex = 0;
    const stCabin_Flash *cabinC = NULL;
    stCabin_Ram *cabinP = NULL;
    enPersonaAI predictedPersona = rfP->AI.predictedPersona;//(enPersonaAI)WiFiVariables.AIPersonaSelect; //

    if(rfP->AI.isEnabled != 1)
        return;

    // Check all cabins for FRZ type
    for(cabinIndex = 0; cabinIndex < _NumOfCabin; cabinIndex++)
    {
        cabinC = &rfC->Cabin[cabinIndex];
        cabinP = &rfP->Cabin[cabinIndex];

        if(cabinC->type != _CabinType_FRZ)
            continue;

        if(predictedPersona < NUMBER_OF_PERSONAS)
            cabinP->values.defrost.variableTime += aiFrzDefrostOffSets[predictedPersona];

        if(!cabinP->values.defrostDoorOpenedTimer)
            if(predictedPersona < _PERSONA_BALANCED_HOUSEHOLD)
                cabinP->values.defrost.variableTime += aiFrzDefrostOffSets[_PERSONA_OCCASIONAL_SNACKER];
    }
}


#endif