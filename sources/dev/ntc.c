/** @file       ntc.c
 *  @brief      C file of NTC device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-05
 */
#define FILE_NTC_C

#include "ntc.h"
#include "board/board.h"
#include "../rcos/dev/dev.h"

/**
 *  @addtogroup NTC
 *  @{
 */

    
static DEV_IO_FUNC_INIT(ntcInit);
static DEV_IO_FUNC_DEINIT(ntcDeinit);
static DEV_IO_FUNC_GET(ntcGet);
uint32_t sensorErrorDetect(tsNtcParams *params, const tsNtcConsts *consts, unsigned short lowerBoundary, unsigned short higherBoundary);

float_t ambientTemp = 0.0;

#define Fair_SENSOR(ADC) (((1 / ((log((((float_t)ADC/1024*5 * 20940) / (5  - (float_t)ADC/1024*5)) / 18900) / 3770) + 1/(273.15 + (-20) ))) - 273.15)*10)
#define Rair_SENSOR(ADC) (((1 / ((log((((float_t)ADC/1024*5 * 5500)  / (5  - (float_t)ADC/1024*5)) / 6350)  / 3830) + 1/(273.15 + (0)   ))) - 273.15)*10)
#define Feva_SENSOR(ADC) (((1 / ((log((((float_t)ADC/1024*5 * 6340)  / (5  - (float_t)ADC/1024*5)) / 3900)  / 3870) + 1/(273.15 + (10)  ))) - 273.15)*10)
#define Reva_SENSOR(ADC) (((1 / ((log((((float_t)ADC/1024*5 * 6340) / (5  - (float_t)ADC/1024*5)) / 3900)  / 3870) + 1/(273.15 + (10)  ))) - 273.15)*10)
#define TC_SENSOR(ADC)   (((1 / ((log((((float_t)ADC/1024*5 * 13300) / (5  - (float_t)ADC/1024*5)) / 10000) / 3900) + 1/(273.15 + (25)  ))) - 273.15)*10)
#define IM_SENSOR(ADC)   (((1 / ((log((((float_t)ADC/1024*5 * 12100) / (5  - (float_t)ADC/1024*5)) / 18900) / 3770) + 1/(273.15 + (-20) ))) - 273.15)*10)
//#define F_SENSOR(ADC) ((( 1 / ((log((((float_t)ADC/1024*5 * Rpu)   / (Vs - (float_t)ADC/1024*5)) / Rspec) /   B ) + 1/(273.15 + Tspec ))) - 273.15)*10)
//                                        Vntc                        Vntc                                                      floating point removing
//These formulas are applied with specific value of Rpullup, Rspec, Tspec

#define ADC_OC_VALUE_FAir 16
#define ADC_SC_VALUE_FAir 955  
#define ADC_OC_VALUE_FEva 48
#define ADC_SC_VALUE_FEva 1002  
#define ADC_OC_VALUE_RAir 56
#define ADC_SC_VALUE_RAir 894
#define ADC_OC_VALUE_REva 48
#define ADC_SC_VALUE_REva 1002
#define ADC_OC_VALUE_TC 108
#define ADC_SC_VALUE_TC 937  
#define ADC_OC_VALUE_IM 16
#define ADC_SC_VALUE_IM 955
#define ADC_OC_VALUE_HS 100
#define ADC_SC_VALUE_HS 615
  
#define ErrorMessage     if(params->error == eNTCErrorOpenCircuit) \
                          return (uint32_t)-650;                   \
                         if(params->error == eNTCErrorShortCircuit)\
                          return (uint32_t) 950;                   \
                            
/// @brief  Structure that defines the functions for ntc
const tsDevIoFuncs devIoNtcFuncs =
    {
        ntcInit,
        ntcDeinit,
        ntcGet,
        NULL,
};

/// @brief  Init function for a ntc
static DEV_IO_FUNC_INIT(ntcInit)
{
    const tsNtcConsts *consts = device->constants;
    UNUSED(config); // REMOVE IF USED

    if (device->sys->initialized == 0)
    {
        device->sys->initialized = 1;
        devIoInit(consts->adcDevice, NULL);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

/// @brief  Deinit function for a ntc
static DEV_IO_FUNC_DEINIT(ntcDeinit)
{
    const tsNtcConsts *consts = device->constants;

    if (device->sys->initialized)
    {
        device->sys->initialized = 0;
        devIoDeinit(consts->adcDevice);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

float_t H_SENSOR(uint16_t adcValue)
{
  
  uint32_t ambientint = (uint32_t) ((int32_t) ambientTemp);
  float_t rh_15 = 36.36 * ((adcValue / 1023.0) * 5.0) - 1.82;
  float_t rh_30 = 36.36 * ((adcValue / 1023.0) * 5.0) - 9.09;
  float_t humidity = 0;
  
    if (ambientint <= 150)
      humidity = rh_15;
    else if (ambientint<=300)
      humidity = rh_15 + ((ambientTemp/10 - 15.0) / 15.0) * (rh_30 - rh_15);
    else 
      humidity = rh_30;
    
    if ((uint32_t) ((int32_t)humidity)<=0)
      return 0;
    else if ((uint32_t) ((int32_t)humidity)<=100)
      return humidity;
    else 
      return 100;
    
  
}

uint32_t sensorErrorDetect(tsNtcParams *params, const tsNtcConsts *consts, unsigned short lowerBoundary, unsigned short higherBoundary)
{
  uint32_t adcValue = devIoGet(consts->adcDevice);
  if (adcValue < lowerBoundary)
  {
    params->error = eNTCErrorOpenCircuit;
  }
  else if (adcValue > higherBoundary)
  {
    params->error = eNTCErrorShortCircuit;
  }
  else
    params->error = eNTCErrorNone;
  
  return adcValue;
  
}
/// @brief  Get function for a ntc
static DEV_IO_FUNC_GET(ntcGet)
{


    tsNtcParams *params = device->parameters;
    const tsNtcConsts *consts = device->constants;
    uint32_t adcValueT = 0;
    

    
    if(device == ntcs[0])      //ntcRSEN: 
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_RAir, ADC_SC_VALUE_RAir);
      ErrorMessage    
      return (uint32_t) ((int32_t)(Rair_SENSOR(adcValueT)));
    }
    else if(device == ntcs[1])  //ntcRDSEN:
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_REva, ADC_SC_VALUE_REva);
      ErrorMessage
      return (uint32_t) ((int32_t)(Reva_SENSOR(adcValueT)));
    }
    else if(device == ntcs[2])  //ntcFSEN:
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_FAir, ADC_SC_VALUE_FAir);
      ErrorMessage
      return (uint32_t) ((int32_t)(Fair_SENSOR(adcValueT)));
    }
    else if(device == ntcs[3])  //ntcFDSEN:
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_FEva, ADC_SC_VALUE_FEva);
      ErrorMessage
      return (uint32_t) ((int32_t)(Feva_SENSOR(adcValueT)));  
    } 
    else if(device == ntcs[4])  //ntcIMSEN:
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_IM, ADC_SC_VALUE_IM); 
      ErrorMessage
      return (uint32_t) ((int32_t)(IM_SENSOR(adcValueT))); 
    }
    else if(device == ntcs[5])  //ntcTCSEN:
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_TC, ADC_SC_VALUE_TC);
      ErrorMessage
      ambientTemp = TC_SENSOR(adcValueT);
      return (uint32_t) ((int32_t)(TC_SENSOR(adcValueT)));
    }
    else if(device == ntcs[6])  //ntcHSEN:
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_HS, ADC_SC_VALUE_HS);
      ErrorMessage
      return (uint32_t) ((int32_t)(H_SENSOR(adcValueT)));
    }
    else if(device == ntcs[7])  //ntcXSEN:
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_FAir, ADC_SC_VALUE_FAir);
      ErrorMessage
      return (uint32_t) ((int32_t)(Fair_SENSOR(adcValueT)));
    } 
    else if(device == ntcs[8])  //ntcRML:
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_FAir, ADC_SC_VALUE_FAir);
      ErrorMessage
      return (uint32_t) ((int32_t)(Fair_SENSOR(adcValueT)));
    }
    else if(device == ntcs[9])  //ntcIMFL:
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_FAir, ADC_SC_VALUE_FAir);
      ErrorMessage
      return (uint32_t) ((int32_t)(Fair_SENSOR(adcValueT)));
    }
    else if(device == ntcs[10]) //ntcKML:
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_FAir, ADC_SC_VALUE_FAir);
      ErrorMessage
      return (uint32_t) ((int32_t)(Fair_SENSOR(adcValueT)));
    }
    else if(device == ntcs[11]) //ntcXML:
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_FAir, ADC_SC_VALUE_FAir);
      ErrorMessage
      return (uint32_t) ((int32_t)(Fair_SENSOR(adcValueT)));
    }
    else if(device == ntcs[12]) //ntcWD_IN:
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_FAir, ADC_SC_VALUE_FAir);
      ErrorMessage
      return (uint32_t) ((int32_t)(Fair_SENSOR(adcValueT)));
    }
    else if(device == ntcs[13]) //ntcIMTEL:
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_FAir, ADC_SC_VALUE_FAir);
      ErrorMessage
      return (uint32_t) ((int32_t)(Fair_SENSOR(adcValueT)));
    }
#ifdef ALPS_H50    
    else if(device == ntcs[14]) //ntcASIN:
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_FAir, ADC_SC_VALUE_FAir);
      ErrorMessage
      return (uint32_t) ((int32_t)(Fair_SENSOR(adcValueT)));
    }
#endif    
    else                        //anyelse
    {
      adcValueT = sensorErrorDetect(params, consts, ADC_OC_VALUE_FAir, ADC_SC_VALUE_FAir);
      ErrorMessage
      return (uint32_t) ((int32_t)(Fair_SENSOR(adcValueT)));
    }
    

/*    const tsDevIo *ntcs[] = {
	0 &ntcRSEN, 
	1 &ntcRDSEN,
	2 &ntcFSEN,
	3 &ntcFDSEN,
	4 &ntcIMSEN,
	5 &ntcTCSEN,
	6 &ntcHSEN,
	7 &ntcRML,
	8 &ntcIMFL,
	9 &ntcKML,
        10 &ntcIMTEL,
};*/
      
  /*  switch (devNtc)
    { 
      case ntcs[0]: //ntcRSEN:
        return (uint32_t) ((int32_t)(Rair_SENSOR(adcValue)));
        break;
      case ntcs[1]: //ntcRDSEN:
        return (uint32_t) ((int32_t)(Reva_SENSOR(adcValue)));
        break;
      case ntcs[2]: //ntcFSEN:
        return (uint32_t) ((int32_t)(Fair_SENSOR(adcValue)));
        break;
      case ntcs[3]: //ntcFDSEN:
        return (uint32_t) ((int32_t)(Feva_SENSOR(adcValue)));
        break;
      case ntcs[5]: //ntcTCSEN:
        return (uint32_t) ((int32_t)(Feva_SENSOR(adcValue)));
        break;
      case ntcs[4]: //ntcIMSEN:
      case ntcs[6]: //ntcHSEN:
      case ntcs[7]: //ntcRML:
      case ntcs[8]: //ntcIMFL:
      case ntcs[9]: //ntcKML:
      case ntcs[10]: //ntcIMTEL:
        return (uint32_t) ((int32_t)(Reva_SENSOR(adcValue)));
        break;
        
      default:  
        return (uint32_t) ((int32_t)(Reva_SENSOR(adcValue)));
        break;
    }*/
    
   // denas = /*(uint32_t) ((int32_t)*/(Rair_SENSOR(value12)*10/*)*/);
    
    //denas = (int32_t)((1 / ((log10(((value12/1024*5 * 20.94)/(5 - value12/1024*5)) / 18.90) / 3770) + (1/(273.15 + (-20))))) - 273.15);
  //  return (uint32_t) ((int32_t)(Rair_SENSOR(value12)));
   // return (uint32_t)((int32_t)adc10BitToCLut[adcValue - 36]);
}


static DEV_IO_FUNC_GET(ntcErrorDetectorGet);

const tsDevIoFuncs devIoNtcErrorDetectorFuncs =
    {
        NULL,
        NULL,
        ntcErrorDetectorGet,
        NULL,
};

/// @brief Gives the error status of a NTC device which the error detector is associated to it.
static DEV_IO_FUNC_GET(ntcErrorDetectorGet)
{

    tsNtcParams *ntcParams = ((tsNtcErrorDetectorConsts *)device->constants)->ntcDevice->parameters;
    return ntcParams->error;
}


/** @} */
