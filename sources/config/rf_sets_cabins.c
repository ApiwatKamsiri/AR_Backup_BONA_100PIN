#include "app/cabin/cabin_sets.h"
#include "app/refrigerator/rf_configs.h"
#include "dev/display_spi.h"

#define _dispQFF    1
#define _dispQFRZ   -25
#define _dispVAC    eDisplaySpecialsMidLine
#define _dispFRZOFF eDisplaySpecialsMidLine

const stCabinSetsTemperature_Flash fahrenheitCelciusTable = {    
    .QFRD_ValueCelcius = 1,
    .QFRZ_ValueCelcius = -25,
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
/* 0*/  { (unsigned char) _dispQFF  , 15     , 10    , -30    , 32    , 0    , 0    , 0    , 0    },//Quick Cooling
/* 1*/  { (unsigned char) _dispVAC  , 0      , 0      , 0      , 0     , 0    , 0    , 0    , 0    },//Vacation Dial
/* 2*/  { (unsigned char) _dispQFRZ , -350   , -360   , -70    , -80   , 0    , 0    , 0    , 0    },//Quicl FRZ
                                     //  Air-Sensor  // EVP-Sensor*Fan    // For Heater
/* 3*/  { (unsigned char) 1         , 15   , 10   , -30    , 32 , 0     , 0     , 0     ,  0   },
/* 4*/  { (unsigned char) 2         , 20   , 15   , -30    , 32 , 0     , 0     , 0     ,  0   },
/* 5*/  { (unsigned char) 3         , 27   , 22   , -30    , 32 , 0     , 0     , 0     ,  0   },
/* 6*/  { (unsigned char) 4         , 40   , 30   , -30    , 32 , 0     , 0     , 0     ,  0   },
/* 7*/  { (unsigned char) 5         , 45   , 35   , -30    , 32 , 0     , 0     , 0     ,  0   },
/* 8*/  { (unsigned char) 6         , 55   , 45   , -30    , 32 , 0     , 0     , 0     ,  0   },
/* 9*/  { (unsigned char) 7         , 60   , 55   , -30    , 32 , 0     , 0     , 0     ,  0   },
/*10*/  { (unsigned char) 8         , 65   , 60   , -30    , 32 , 0     , 0     , 0     ,  0   },
  
/*11*/  { (unsigned char)-22        , -250  , -260  , -70  , -80  , 0     , 0     , 0     ,  0   },
/*12*/  { (unsigned char)-21        , -210  , -230  , -60  , -70  , 0     , 0     , 0     ,  0   },
/*13*/  { (unsigned char)-20        , -180  , -200  , -50  , -60  , 0     , 0     , 0     ,  0   },
/*14*/  { (unsigned char)-19        , -150  , -200  , -40  , -50  , 0     , 0     , 0     ,  0   },
/*15*/  { (unsigned char)-18        , -140  , -190  , -30  , -40  , 0     , 0     , 0     ,  0   },
/*16*/  { (unsigned char)-17        , -130  , -180  , -20  , -30  , 0     , 0     , 0     ,  0   },
/*17*/  { (unsigned char)-16        , -120  , -170  , -10  , -20  , 0     , 0     , 0     ,  0   },                                   


/*18*/  { (unsigned char)-16        , -170  , -172  , -100  , -350  , 0     , 0     , 0     ,  0   },//No exist***

/*19*/  { (unsigned char) 8         , 25    , 15    , 25    , 15    , 0     , 0     , 0     ,  0   },
/*20*/  { (unsigned char) _dispFRZOFF , 138 , 115   , 138   , 115   , 0     , 0     , 0     ,  0   },};

