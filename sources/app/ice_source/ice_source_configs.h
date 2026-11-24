#ifndef ICE_SOURCE_CONFIGS_H
#define ICE_SOURCE_CONFIGS_H

#include "app/refrigerator/components_configs.h"

// TODO : Create Ice source device ?
typedef enum{
  _IceSourceType_NONE = 0,
  _IceSourceType_IceCube,
  _IceSourceType_IceCubeAndCrash,
}enIceSourceTypes;

typedef struct{
  stSelector_Flash selector;
  stShutter_Flash shutter;
  stPusher_Flash pusher;
}stIceSourceComponents_Flash;

typedef struct{
  enIceSourceTypes type:2;
  stIceSourceComponents_Flash components;
}stIceSource_Flash;


#endif