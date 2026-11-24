#include "app/cabin/cabin_sets.h"
#include "app/refrigerator/rf_configs.h"
#include "dev/display_spi.h"

#define _dispQFF    2
#define _dispQFRZ   -24
#define _dispVAC    eDisplaySpecialsMidLine
#define _dispFRZOFF eDisplaySpecialsMidLine

const stCabinSetsTemperature_Flash fahrenheitCelciusTable = {    
    .QFRD_ValueCelcius = 2,
    .QFRZ_ValueCelcius = -24,
    .lookUpTable = 
    {  //Fahrenheit   celcius      pointer
      
      {39,             4} ,        //0
      {38,             4} ,        //1
      {37,             3} ,        //2
      {36,             3} ,        //3
      {35,             2} ,        //4
      {34,             2} ,        //5
      {33,             1} ,        //6
      {46,             8} ,        //7
      {45,             8} ,        //8
      {44,             7} ,        //9
      {43,             7} ,        //10
      {42,             6} ,        //11 
      {41,             5} ,        //12
      {40,             5} ,        //13
 
      {3,             -16} ,       //14      
      {2,             -16} ,       //15      
      {1,             -17} ,       //16      
      {0,              -17} ,      //17
      {-1,             -18} ,      //18
      {-2,             -19} ,      //19
      {-3,             -20} ,      //20
      {-4,             -20} ,      //21
      {-5,             -21} ,      //22
      {-6,             -21} ,      //23
      {-7,             -22} ,      //24
      {-8,             -22} ,      //25
      {-9,             -23} ,      //26
      {-10,            -23} ,      //27
      {-11,            -24} ,      //28
      {-12,            -24} ,      //29
      {-13,            -25} ,      //30
      {-14,            -25} ,      //31
      
      {-16,            -27} ,      //32
    },
};

#pragma location = 0x00037000 + sizeof(stRefrigerator_Flash) //APW New MCU
const stCabinSetsDatas_Flash cabinSetsDatasTable_F[] = {                                //<-------- Door Open---------><-------Door Close--------->
//      {                 display   , c_CI_Temp   , c_CO_Temp   , f_CI_Temp   , fCO_Temp    , lTH_CI_Temp , lTH_CO_Temp , lTH_CI_Temp , lTH_CO_Temp },
//      {                 display   , c_CI_Temp   , c_CO_Temp   , f_CI_Temp   , fCO_Temp    , lTH_CI_Temp , lTH_CO_Temp , lTH_CI_Temp , lTH_CO_Temp },
/* 0*/  { (unsigned char) _dispQFF  , -140  , -150  , -140  , -150  , 0    , 0    , 0    , 0    },//Quick Cooling
/* 1*/  { (unsigned char) _dispVAC  , 0     , 0     , 0     , 0     , 0    , 0    , 0    , 0    },//Vacation Dial
/* 2*/  { (unsigned char) _dispQFRZ , -465  , -475  , -465  , -475  , 0    , 0    , 0    , 0    },//Quicl FRZ
                                     //  Air-Sensor  // EVP-Sensor*Fan    // For Heater
/* 3*/  { (unsigned char) 2         , 10   , 0    , 10   , 0   , 0     , 0     , 0     ,  0   },        //Max
/* 4*/  { (unsigned char) 3         , 20   , 0    , 20   , 0   , 0     , 0     , 0     ,  0   },
/* 5*/  { (unsigned char) 4         , 25   , 5    , 25   , 5  , 0     , 0     , 0     ,  0   },        //Mid Max
/* 6*/  { (unsigned char) 5         , 36   , 16   , 36   , 16  , 0     , 0     , 0     ,  0   },
/* 7*/  { (unsigned char) 6         , 47   , 27   , 47   , 27  , 0     , 0     , 0     ,  0   },        //Mid Min
/* 8*/  { (unsigned char) 7         , 59   , 39   , 59   , 39  , 0     , 0     , 0     ,  0   },
/* 9*/  { (unsigned char) 8         , 70   , 50   , 70   , 50  , 0     , 0     , 0     ,  0   },        //Min

/*10*/  { (unsigned char) 0         ,  0   ,  0   ,  0   ,  0  , 0     , 0     , 0     ,  0   },        //Not Use
/*11*/  { (unsigned char) 0         ,  0   ,  0   ,  0   ,  0  , 0     , 0     , 0     ,  0   },        //Not Use

/*12*/  { (unsigned char) -24        , -255  , -265  , -255  , -265  , 0     , 0     , 0     ,  0   },    //Max
/*13*/  { (unsigned char) -22        , -235  , -245  , -235  , -245  , 0     , 0     , 0     ,  0   },
/*14*/  { (unsigned char) -20        , -215  , -225  , -215  , -225  , 0     , 0     , 0     ,  0   },    //Mid Max
/*15*/  { (unsigned char) -19        , -203  , -213  , -203  , -213  , 0     , 0     , 0     ,  0   },
/*16*/  { (unsigned char) -18        , -190  , -200  , -190  , -200  , 0     , 0     , 0     ,  0   },    //Mid Min
/*17*/  { (unsigned char) -17        , -178  , -188  , -178  , -188  , 0     , 0     , 0     ,  0   },
/*18*/  { (unsigned char) -16        , -165  , -175  , -165  , -175  , 0     , 0     , 0     ,  0   },    //Min

/*19*/  { (unsigned char)   0        ,    0  ,    0  ,    0  ,    0  , 0     , 0     , 0     ,  0   },    //Not Use
/*20*/  { (unsigned char)   0        ,    0  ,    0  ,    0  ,    0  , 0     , 0     , 0     ,  0   }, }; //Not Use

