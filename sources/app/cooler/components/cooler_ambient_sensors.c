//#include "cooler_ambient_sensors.h"
#include "cooler_components.h"

void coolerUpdateAmbientType(const stCooler_Flash *coolerC, stCooler_Ram *coolerP)
{
  signed short ambientTemp;
  enAmbientType ambientType = coolerP->parameters.ambientType;

  if ((coolerC->components.ambientTempSensor.type == _TempSensorType_NONE))
  //&& (params->rf->Values.I2CDevices.device.HumiditySensor == 0))
  {
    coolerP->parameters.ambientType = _Ambient_NONE;
    coolerP->components.ambientTempSensor.error = eNTCErrorNone;
    return;
  }
  else
  {
    if (coolerP->components.ambientTempSensor.error)
    {
      coolerP->parameters.ambientType = coolerC->parameters.ambient.errorType;
      return;
    }
  }

  ambientTemp = coolerP->components.ambientTempSensor.valueInCExt;

  switch (ambientType)
  {
  case _Ambient_0:
    if (ambientTemp >= coolerC->parameters.ambient.temperatures[_Ambient_0] + coolerC->parameters.ambient.hysteresis[_Ambient_0])
      ambientType = _Ambient_1;
    break;

  case _Ambient_1:
    if (ambientTemp < coolerC->parameters.ambient.temperatures[_Ambient_0] - coolerC->parameters.ambient.hysteresis[_Ambient_0])
      ambientType = _Ambient_0;
    else if (ambientTemp >= coolerC->parameters.ambient.temperatures[_Ambient_1] + coolerC->parameters.ambient.hysteresis[_Ambient_1])
      ambientType = _Ambient_2;
    break;

  case _Ambient_2:
    if (ambientTemp < coolerC->parameters.ambient.temperatures[_Ambient_1] - coolerC->parameters.ambient.hysteresis[_Ambient_1])
      ambientType = _Ambient_1;
    else if (ambientTemp >= coolerC->parameters.ambient.temperatures[_Ambient_2] + coolerC->parameters.ambient.hysteresis[_Ambient_2])
      ambientType = _Ambient_3;
    break;

  case _Ambient_3:
    if (ambientTemp < coolerC->parameters.ambient.temperatures[_Ambient_2] - coolerC->parameters.ambient.hysteresis[_Ambient_2])
      ambientType = _Ambient_2;
    else if (ambientTemp >= coolerC->parameters.ambient.temperatures[_Ambient_3] + coolerC->parameters.ambient.hysteresis[_Ambient_3])
      ambientType = _Ambient_4;
    break;

  case _Ambient_4:
    if (ambientTemp < coolerC->parameters.ambient.temperatures[_Ambient_3] - coolerC->parameters.ambient.hysteresis[_Ambient_3])
      ambientType = _Ambient_3;
    break;

  case _Ambient_NONE:
    if (ambientTemp < coolerC->parameters.ambient.temperatures[_Ambient_0])
      ambientType = _Ambient_0;
    else if (ambientTemp >= coolerC->parameters.ambient.temperatures[_Ambient_0] && ambientTemp < coolerC->parameters.ambient.temperatures[_Ambient_1])
      ambientType = _Ambient_1;
    else if (ambientTemp >= coolerC->parameters.ambient.temperatures[_Ambient_1] && ambientTemp < coolerC->parameters.ambient.temperatures[_Ambient_2])
      ambientType = _Ambient_2;
    else if (ambientTemp >= coolerC->parameters.ambient.temperatures[_Ambient_2] && ambientTemp < coolerC->parameters.ambient.temperatures[_Ambient_3])
      ambientType = _Ambient_3;
    else if (ambientTemp >= coolerC->parameters.ambient.temperatures[_Ambient_3])
      ambientType = _Ambient_4;

    break;

  default:
    ambientType = _Ambient_NONE;
    break;
  }

   coolerP->parameters.ambientType = ambientType;//ambientType;
}

void coolerUpdateHumidityType(const stCooler_Flash *coolerC, stCooler_Ram *coolerP)
{
  if (coolerC->components.humiditySensor.type == _HumSensorType_NONE)
  {
    coolerP->parameters.humidityType = _Humidity_NONE;//_Humidity_NONE;
    coolerP->components.humiditySensor.error = 0;
    return;
  }
  else
  {
    if (coolerP->components.humiditySensor.error)
    {
      coolerP->parameters.humidityType = coolerC->parameters.ambient.humidity.errorType;
      return;
    }
  }

  enHumidityType humidityType = coolerP->parameters.humidityType;
  if(((coolerP->components.solenoidValve[0].positionIndex == _FRZ_OPEN) && (coolerP->parameters.humidityCheckTimer > 5))
     || (!coolerP->components.humiditySensor.humidityValueCal))
    coolerP->components.humiditySensor.humidityValueCal = coolerP->components.humiditySensor.humidityValue;
  
  unsigned char humidity = coolerP->components.humiditySensor.humidityValueCal;

  switch (humidityType)
  {
  case _Humidity_0:
    if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_0] + coolerC->parameters.ambient.humidity.hysteresis[_Humidity_0])
      humidityType = _Humidity_1;
    break;

  case _Humidity_1:
    if (humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_0] - coolerC->parameters.ambient.humidity.hysteresis[_Humidity_0])
      humidityType = _Humidity_0;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_1] + coolerC->parameters.ambient.humidity.hysteresis[_Humidity_1])
      humidityType = _Humidity_2;
    break;

  case _Humidity_2:
    if (humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_1] - coolerC->parameters.ambient.humidity.hysteresis[_Humidity_1])
      humidityType = _Humidity_1;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_2] + coolerC->parameters.ambient.humidity.hysteresis[_Humidity_2])
      humidityType = _Humidity_3;
    break;

  case _Humidity_3:
    if (humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_2] - coolerC->parameters.ambient.humidity.hysteresis[_Humidity_2])
      humidityType = _Humidity_2;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_3] + coolerC->parameters.ambient.humidity.hysteresis[_Humidity_3])
      humidityType = _Humidity_4;
    break;

  case _Humidity_4:
    if (humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_3] - coolerC->parameters.ambient.humidity.hysteresis[_Humidity_3])
      humidityType = _Humidity_3;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_4] + coolerC->parameters.ambient.humidity.hysteresis[_Humidity_4])
      humidityType = _Humidity_5;
    break;
    
    case _Humidity_5:
    if (humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_4] - coolerC->parameters.ambient.humidity.hysteresis[_Humidity_4])
      humidityType = _Humidity_4;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_5] + coolerC->parameters.ambient.humidity.hysteresis[_Humidity_5])
      humidityType = _Humidity_6;
    break;

  case _Humidity_6:
    if (humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_5] - coolerC->parameters.ambient.humidity.hysteresis[_Humidity_5])
      humidityType = _Humidity_5;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_6] + coolerC->parameters.ambient.humidity.hysteresis[_Humidity_6])
      humidityType = _Humidity_7;
    break;

  case _Humidity_7:
    if (humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_6] - coolerC->parameters.ambient.humidity.hysteresis[_Humidity_6])
      humidityType = _Humidity_6;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_7] + coolerC->parameters.ambient.humidity.hysteresis[_Humidity_7])
      humidityType = _Humidity_8;
    break;

  case _Humidity_8:
    if (humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_7] - coolerC->parameters.ambient.humidity.hysteresis[_Humidity_7])
      humidityType = _Humidity_7;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_8] + coolerC->parameters.ambient.humidity.hysteresis[_Humidity_8])
      humidityType = _Humidity_9;
    break;

  case _Humidity_9:
    if (humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_8] - coolerC->parameters.ambient.humidity.hysteresis[_Humidity_8])
      humidityType = _Humidity_8;
    break;

  case _Humidity_NONE:
    if (humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_0])
      humidityType = _Humidity_0;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_0] && humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_1])
      humidityType = _Humidity_1;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_1] && humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_2])
      humidityType = _Humidity_2;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_2] && humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_3])
      humidityType = _Humidity_3;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_3] && humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_4])
      humidityType = _Humidity_4;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_4] && humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_5])
      humidityType = _Humidity_5;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_5] && humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_6])
      humidityType = _Humidity_6;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_6] && humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_7])
      humidityType = _Humidity_7;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_7] && humidity < coolerC->parameters.ambient.humidity.intervals[_Humidity_8])
      humidityType = _Humidity_8;
    else if (humidity >= coolerC->parameters.ambient.humidity.intervals[_Humidity_8])
      humidityType = _Humidity_9;

    break;

  default:
    humidityType = _Humidity_NONE;
    break;
  }

  coolerP->parameters.humidityType = humidityType;
}
