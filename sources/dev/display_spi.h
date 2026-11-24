/** @file       display.h
 *  @brief      Header file of DISPLAY device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-11
 *  @details
 *  This device is used to control and communicate with refrigerater displays
 *  This includes:
 *  1- Controlling the 7-segment area of a cabin by displaying a signed number or a special character
 *  2- Turning function leds on or off
 *  3- Playing melodies on the buzzer of the display
 *  4- Getting display info (ver, rev, ID)
 *  5- Getting the event occurred on the display like button X is pressed
 *  6- Controlling the dimming level of the display
 * 
 *  @code
 *  // Creation ...
 *  #include "dev/spi.h"
 *  DEV_COM_spi_CREATE(spi, SPI0, Num1, 32, 32)
 * 
 *  #include "dev/display_spi.h"
 *  DEV_CPX_DISPLAY_SPI_CREATE(display, displaySpi, 10)
 * 
 *  // Initialization
 *  devCpxInit(&display, NULL);
 * 
 *  // Usage...
 *  devDspSetCabinData(&display, eDisplayCabin0, eDisplayCabinDataTypeNumber, -18);
 *  devDspSetCabinData(&display, eDisplayCabin0, eDisplayCabinDataTypeSpecial, eDisplaySpecialsError);
 *  devDspSendCommand(&display, eMainBoardCommandBuzzerMDoorsAlert);
 *  devDspSetLedState(&display, eDisplayLedVacation, eDisplayLedStateOn);
 *  devDspSetDimLevel(&display, eDisplayDimLevel75);
 * 
 *  teDisplayEvent ev = devDspGetEvent(&display);
 *  if (devDspGetId(&display) == 0xA0) {}
 * 
 * 
 */
#ifndef FILE_DISPLAY_H
#define FILE_DISPLAY_H

/// Includes
#include "rcos.h"
#include "system.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_DISPLAY_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   DISPLAY DISPLAY
 *  @ingroup    DEV_CPX
 *  @brief      Driving spi displays of the refrigerator
 *  @{
 */

/// @brief  Functions for DISPLAY devices
INTERFACE const tsDevCpxFuncs devCpxDisplayFuncs;

typedef enum
{
    eMainBoardCommandNone = 0,
    eMainBoardCommandBuzzerMOpen,
    eMainBoardCommandBuzzerMClose,
    eMainBoardCommandBuzzerMStart,
    eMainBoardCommandBuzzerMEnd,
    eMainBoardCommandBuzzerMButton,
    eMainBoardCommandBuzzerMWarning,
    eMainBoardCommandBuzzerMDoorsAlert = 10,
} teMainBoardCommand; ///< Commands can be sent from main board to display

/* clang-format off */
typedef enum 
{
    eDisplayEventNone                   = 0,
    eDisplayEventBuzzer                 = 1,
    eDisplayEventC0SetUp                = 2,
    eDisplayEventC1SetUp                = 3,
    eDisplayEventC2SetUp                = 4,
    eDisplayEventC3SetUp                = 5,
    eDisplayEventC4SetUp                = 6,

    eDisplayEventC0SetDown              = 7,            /*** BONA(FF set) ***/ // Cleverout display temperature set button sends 0x08 for FF,
    eDisplayEventC1SetDown              = 8,            /*** BONA(FRZ set) ***/
    eDisplayEventC2SetDown              = 9,
    eDisplayEventC3SetDown              = 10,
    eDisplayEventC4SetDown              = 11,

    eDisplayEventC0QCool                = 12,           /*** BONA(FF Quick set) ***/
    eDisplayEventC1QCool                = 13,           /*** BONA(FRZ Quick set) ***/
    eDisplayEventC2QCool                = 14,
    eDisplayEventC3QCool                = 15,
    eDisplayEventC4QCool                = 16,

    eDisplayEventC0Mode                 = 17,
    eDisplayEventC1Mode                 = 18,
    eDisplayEventC2Mode                 = 19,
    eDisplayEventC3Mode                 = 20,   
    eDisplayEventC4Mode                 = 21,

    eDisplayEventHighTempAlarmOff        = 22, 			//eDisplayEventDispOff = 22,//eDisplayEventhighTemp=22,
    eDisplayEventDispDimmer              = 23,
    eDisplayEventKeyLock                 = 24,          /*** BONA(Child lock) ***/
    eDisplayEventEcoExtra                = 25,          /*** BONA(ECO) ***/
    eDisplayEventVacation                = 26,
    eDisplayEventRice                    = 27, 			//eDisplayEventLarder=27,
    eDisplayEventFilterReset             = 28,
    eDisplayEventIceOff                  = 29,
    eDisplayEventSabbath                 = 30,          /*** BONA(Shabbath) ***/
    eDisplayEventDispencer               = 31,
    eDisplayEventIceCreamMchSet          = 32,
    eDisplayEventIceCreamMchOnOff        = 33,

    eDisplayEventStateMenu               = 34,          /*** BONA(swith function in service mode) ***/
    eDisplayEventPassword                = 35,          /*** BONA(Enable service mode) ***/
    eDisplayEventComponentsOn            = 36, 			//eDisplayEventSpecialsOnOff=36,
    eDisplayEventComponentsOff           = 37, 			//eDisplayEventALLOff=37,
    eDisplayEventTempUnitChange          = 38,          /*** BONA(Switch C to F, F to C) ***/
    eDisplayEventSelectCabin             = 39,  
    eDisplayEventSetCabin                = 40,
    eDisplayEventSilent                  = 41,
    eDisplayEventApplianceOFF            = 42,          /*** BONA(App off) ***/
    eDisplayEventActivateCabins          = 43,
    eDisplayEventActivateFunctions       = 44,
    eDisplayEventIncreasingSetValues     = 45,
    eDisplayEventDecreasingSetValues     = 46,
    eDisplayEventOK                      = 47,
    eDisplayEventWifiStartStop           = 48,          /*** BONA(Wifi Enable/Disable) ***/
    eDisplayEventWifiConfig              = 49,          /*** BONA(Wifi Config) ***/
    eDisplayEventRealTemperaturecabin0   = 50,
    eDisplayEventRealTemperaturecabin1   = 51,
    eDisplayEventRealTemperaturecabin2   = 52,
    eDisplayEventDoorOpened              = 53,
    eDisplayEventWifiReset               = 54, 			//wifi reset tus kombinasyonu
    eDisplayEventAutoDoor                = 55,
    eDisplayEventChangeModeJokerCabin    = 56,
    eDisplayEventFastBottleCooling       = 57,
    eDisplayEventConverterFRZtoFF        = 78,   
    eDisplayEventDispOff                 = 81, 			//eDisplayEventDispOffManuel=81,
    eDisplayEventWifiFactoryReset        = 82,          /*** BONA(Wifi Factory Reset) ***/
    eDisplayEventConverterFRZtoOFF       = 83,
    eDisplayEventServiceButton           = 85,
    eDisplayEventIceOn                   = 90,          /*** BONA(Ice making enable) ***/
    eDisplayEventHandleLight             = 91,          /*** BONA(Handle Light) ***/
    eDisplayEventAutoTest                = 95,
    eDisplayEventMeasuredFill            = 96,
    eDisplayEventCalibrateCustom         = 97,
    eDisplayEventWater                   = 98,
    eDisplayEventLightOff                 =100,
    eDisplayEventSwitchMode               =101,
    eDisplayEventIceCrush                = 102,             
    eDisplayEventIceCube                 = 103,
    eDisplayEventQuickIce60              = 128,         /*** BONA(Quick Ice making enable) ***/
    eDisplayEventVacuum                  = 129,         
    eDisplayEventCtMode                  = 130,
    eDisplayEventEcoWhirlpool            = 132,			/*** HOSHI H30(Eco Mode) ***/
    eDisplayEventMuteBuzzer              = 134,			/*** HOSHI H30(Mute Mode) ***/
    eDisplayEventDemoMode                = 135,			/*** HOSHI H30(Demo Mode) ***/
    eDisplayEventServiceMode             = 136,			/*** HOSHI H30(Service Mode AHTH) ***/
    eDisplayEventFailureHistoryClear     = 137,			/*** HOSHI H30(Failure clear) ***/
    eDisplayEventTempSettingFine         = 138,			/*** HOSHI H30(Enable 9 Step) ***/
    eDisplayEventIceTest                 = 145,         /*** HOSHI(Ice making component start test) ***/
    eDisplayEventMax
}  teDisplayEvent;   ///< Events can be sent from display to main board
/* clang-format on */

typedef enum
{
    eDisplayCabinStart = 0,
    eDisplayCabin0 = eDisplayCabinStart, // FF cabin
    eDisplayCabin1,                      // Frz cabin
    eDisplayCabin2,                      // joker cabin
    eDisplayCabin3,
    eDisplayCabin4,
    eDisplayCabinIceCream,
    eDisplayCabinsCount,
} teDisplayCabins;

#define DSP_USED_CABINS_COUNT   4

typedef enum
{
    eDisplayCabinDataTypeNumber = 0,  ///< diplay signed number on cabin 7-segments
    eDisplayCabinDataTypeSpecial = 1, ///< display special character on cabin 7-segments
    eDisplayCabinDataTypesCount,
} teDisplayCabinDataTypes;

enum enumDisplayCabinSpecialData // character 
{
    eDisplaySpecialsStart = 0,
    //FF
    eDisplaySpecialsFFAir = eDisplaySpecialsStart,
    eDisplaySpecialsFFEva,
    eDisplaySpecialsFFExtra,
    eDisplaySpecialsFFInnerLight,
    eDisplaySpecialsFFFan,
    eDisplaySpecialsFFFlap,
    eDisplaySpecialsFFLowTempHeater,

    // FRZ
    eDisplaySpecialsFRZAir,
    eDisplaySpecialsFRZEva,
    eDisplaySpecialsFRZExtra,
    eDisplaySpecialsFRZInnerLight,
    eDisplaySpecialsFRZFan,
    eDisplaySpecialsFRZFlap,
    eDisplaySpecialsFRZDefrostHeater,

    // JKR
    eDisplaySpecialsJKRAir,
    eDisplaySpecialsJKREva,
    eDisplaySpecialsJKRExtra,
    eDisplaySpecialsJKRInnerLight,
    eDisplaySpecialsJKRFan,
    eDisplaySpecialsJKRFlap,
    eDisplaySpecialsJKRLowTempHeater,
    eDisplaySpecialsJKRDefrostHeater,

    // FLAP
    eDisplaySpecialsFLAPAir,
    eDisplaySpecialsFLAPEva,
    eDisplaySpecialsFLAPExtra,
    eDisplaySpecialsFLAPInnerLight,
    eDisplaySpecialsFLAPFan,
    eDisplaySpecialsFLAPFlap,
    eDisplaySpecialsFLAPLowTempHeater,
    eDisplaySpecialsFLAPChannelHeater,
    eDisplaySpecialsFLAPFlapHeater,

    // BioFresh
    eDisplaySpecialsBioFreshAir,
    eDisplaySpecialsBioFreshInnerLight,
    eDisplaySpecialsBioFreshFan,

    // AllCabin
    eDisplaySpecialsAllHeaterA,
    eDisplaySpecialsAllHeaterB,
    eDisplaySpecialsAllHeaterC,
    eDisplaySpecialsAllBlueLight,
    eDisplaySpecialsAllIonizer,
    eDisplaySpecialsAllVioled, //eDisplaySpecialsAllReserved0,
    eDisplaySpecialsAllConverter,
    eDisplaySpecialsAllReserved2,
    eDisplaySpecialsAllReserved3,
    eDisplaySpecialsAllOTA, // eDisplaySpecialsAllReserved4, //43

    // Cooler
    eDisplaySpecialsCoolerAmbientSensor,
    eDisplaySpecialsCoolerCompressor,
    eDisplaySpecialsCoolerCondenserFan,
    eDisplaySpecialsCoolerSolenoidValve,
    eDisplaySpecialsCoolerReserved0,
    eDisplaySpecialsCoolerReserved1,
    eDisplaySpecialsCoolerReserved2,
    eDisplaySpecialsCoolerReserved3,
    eDisplaySpecialsCoolerReserved4,

    // IceMachine
    eDisplaySpecialsIceMachineTempSensor,
    eDisplaySpecialsIceMachineIceMotor,
    eDisplaySpecialsIceMachineWaterPumpTime,
    eDisplaySpecialsIceMachineReserved,

    // IceSource
    eDisplaySpecialsIceSourceSelector,
    eDisplaySpecialsIceSourceShutter,
    eDisplaySpecialsIceSourcePusher,
    eDisplaySpecialsIceSourceReserved,

    // Dispenser
    eDisplaySpecialsDispenserLight,
    eDisplaySpecialsDispenserReserved,

    // WaterSource
    eDisplaySpecialsWaterSourceMainPump,
    eDisplaySpecialsWaterSourceMainValve,
    eDisplaySpecialsWaterSourceDispenserValve,
    eDisplaySpecialsWaterSourceIceMachineValve,
    eDisplaySpecialsWaterSourceWaterVentilHeater,
    eDisplaySpecialsWaterSourceReserved,

    // IceCreamMachine
    eDisplaySpecialsIceCreamMachineTempSensor,
    eDisplaySpecialsIceCreamMachineMixer,
    eDisplaySpecialsIceCreamMachineReserved,

    // Parameters
    eDisplaySpecialsParametersIceMachineWaterPumpTime,
    eDisplaySpecialsParametersDispenserLightTime,
    eDisplaySpecialsParametersReserved1,
    eDisplaySpecialsParametersReserved2,
    eDisplaySpecialsParametersReserved3,
    eDisplaySpecialsParametersReserved4,

    // Specials
    eDisplaySpecialsAllOff,
    eDisplaySpecialsAllOn,
    eDisplaySpecialsMidLine,
    eDisplaySpecialsError,
    eDisplaySpecialsCDegree,
    eDisplaySpecialsFDegree,
    eDisplaySpecialsOn,
    eDisplaySpecialsOff,
    eDisplaySpecialsSabbath,
    eDisplaySpecialsPassword,
    eDisplaySpecialsCabin,
    eDisplaySpecialsComponents,
    eDisplaySpecialsReserved0,
    eDisplaySpecialsReserved1,
    eDisplaySpecialsCoolerHumiditySensor,
    eDisplaySpecialsAllHandleLight,
    eDisplaySpecialsReserved4,
    eDisplaySpecialsCount,
};

typedef enum
{
    eDisplayDimLevelStart = 1,
    eDisplayDimLevel25 = eDisplayDimLevelStart,
    eDisplayDimLevel75,
    eDisplayDimLevel50,
    eDisplayDimLevel100,
    eDisplayDimLevelsCount,
} teDisplayDimLevels;

typedef union
{
    struct
    {
        // Led Functions Byte 0
        uint8_t ledEconomy : 1;
        uint8_t ledEcoExtra : 1;
        uint8_t ledVacation : 1;
        uint8_t ledIceOFF : 1;
        uint8_t ledAlert : 1;
        uint8_t ledLock : 1;
        uint8_t ledDispOff : 1;
        uint8_t ledFilter : 1;

        // Led Functions Byte 1
        uint8_t ledConverter : 1;
        uint8_t dimLevel : 3; // teDisplayDimLevels
        uint8_t ledSplitIce : 1;
        uint8_t ledCubicIce : 1;
        uint8_t ledWater : 1;
        uint8_t ledRice : 1;

        // Led Functions Byte 2
        uint8_t ledCabin0Quick : 2;
        uint8_t ledCabin1Quick : 2;
        uint8_t ledCabin2Quick : 2;
        uint8_t ledCabin3Quick : 2;

        // Led Functions Byte 3
        uint8_t ledCabin4Quick : 2;
        uint8_t ledIceCream : 2;
        uint8_t ledSabbath : 1;
        uint8_t ledWifi : 1;
        uint8_t ledCelsius : 1;
        uint8_t ledFahrenheit : 1;

        // Led Functions Byte 4
        uint8_t ledCabin0Icon : 1;
        uint8_t ledCabin1Icon : 1;
        uint8_t ledCabin2Icon : 1;
        uint8_t ledCabin3Icon : 1;
        uint8_t ledCabin4Icon : 1;
        uint8_t ledVioled : 1;
        uint8_t ledDispenserSW : 1;
        uint8_t displayOff : 1;

        // Led Functions Byte 5
        uint8_t ledFrzOff : 1;
        uint8_t ledUVC : 1;
        uint8_t ledServiceButton : 1;
        uint8_t ledSoftFRZ : 1;
        uint8_t ledJokerFrz : 1;
        uint8_t ledJokerFF : 1;
        uint8_t ledAutoDoor : 1;
        uint8_t ledFastBottleCooling : 1;

        // Led Functions Byte 6
        uint8_t reserved : 3;
        uint8_t ledIceOn : 1;
        uint8_t ledSplitIceDispOff : 1;
        uint8_t ledCubicIceDispOff : 1;
        uint8_t ledWaterDispOff : 1;
        uint8_t ledApplianceOff : 1;
    };

    uint8_t allFunctions[7];
} tuDisplayFunctions;

typedef union
{
    struct
    {
        // Led Functions Byte 0
        uint8_t ledDeli : 1;
        uint8_t ledBeverage : 1;
        uint8_t ledMeat : 1;
        uint8_t ledFeature4 : 1;
        uint8_t ledFeature5 : 1;
        uint8_t reserved0 : 1;
        uint8_t reserved1 : 1;
        uint8_t reserved2 : 1;

        // Led Functions Byte 1
        uint8_t led8 : 1;
        uint8_t led16 : 1; // teDisplayDimLevels
        uint8_t led32 : 1;
        uint8_t ledCustom : 1;
        uint8_t ledLockDspr : 1;
        uint8_t reserved4 : 1;
        uint8_t reserved5 : 1;
        uint8_t reserved6 : 1;

        // Led Functions Byte 2
        uint8_t reserved7 : 8;
        
        // Led Functions Byte 3
        uint8_t reserved8 : 8;
    };

    uint8_t allFunctions2[4];
} tuDisplayFunctions2;

typedef enum
{
    // Led Functions Byte 0
    eDisplayLedEconomy = 0,
    eDisplayLedEcoExtra = 1,
    eDisplayLedVacation = 2,
    eDisplayLedIceOFF = 3,
    eDisplayLedAlert = 4,
    eDisplayLedLock = 5,
    eDisplayLedDispOff = 6,
    eDisplayLedFilter = 7,

    // Led Functions Byte 1
    eDisplayLedConverter = 8,
    //  dimLevel : 3;
    eDisplayLedSplitIce = 12,
    eDisplayLedCubicIce = 13,
    eDisplayLedWater = 14,
    eDisplayLedRice = 15,

    // Led Functions Byte 2
    eDisplayLedCabin0Quick = 16,
    eDisplayLedCabin1Quick = 18,
    eDisplayLedCabin2Quick = 20,
    eDisplayLedCabin3Quick = 22,

    // Led Functions Byte 3
    eDisplayLedCabin4Quick = 24,
    eDisplayLedIceCream = 26,
    eDisplayLedSabbath = 28,
    eDisplayLedWifi = 29,
    eDisplayLedCelsius = 30,
    eDisplayLedFahrenheit = 31,

    // Led Functions Byte 4
    eDisplayLedCabin0Icon = 32,
    eDisplayLedCabin1Icon = 33,
    eDisplayLedCabin2Icon = 34,
    eDisplayLedCabin3Icon = 35,
    eDisplayLedCabin4Icon = 36,
    eDisplayLedVioled = 37,
    eDisplayLedDispenserSW = 38,
    //  displayOff  : 1

    // Led Functions Byte 5
    eDisplayLedFrzOff = 40,
    eDisplayLedUVC = 41,
    eDisplayLedServiceButton = 42,
    eDisplayLedSoftFRZ = 43,
    eDisplayLedJokerFrz = 44,
    eDisplayLedJokerFF = 45,
    eDisplayLedAutoDoor = 46,
    eDisplayLedFastBottleCooling = 47,

    // Led Functions Byte 6
    // uint8_t reserved : 3;
    eDisplayLedIceOn = 51,
    eDisplayLedSplitIceDispOff = 52,
    eDisplayLedCubicIceDispOff = 53,
    eDisplayLedWaterDispOff = 54,
    eDisplayLedApplianceOff = 55,
} teDisplayLeds;

typedef enum
{
    eDisplayLedStateOff = 0,
    eDisplayLedStateOn = 1,
    eDisplayLedStateAnimation = 2,
    eDisplayLedStateCount,
} teDisplayLedState;

typedef struct
{
    uint8_t id;
    uint8_t swVer;
    uint8_t swRev;
} tsDisplayInfo;

typedef struct
{
    uint8_t LedSet;
    uint8_t BuzzerSet;
    uint8_t AllPeripheralControl;
    uint8_t ResetAll;
} tsDisplayCommandIDInfo;

typedef enum {
    CMD_LED_SET = 1,
    CMD_BUZZER_SET,
    CMD_SWITCH_BUTTON_SET,
    CMD_RESET_ALL
} CommandID;
 


typedef struct PLATFORM_PACKED
{
    int8_t cabinData[eDisplayCabinsCount]; // 6 bytes
    // 6
    uint8_t cabinDataType : eDisplayCabinsCount;
    //uint8_t reserved_0 : 2;                                                   //APW : Update command CtmodeState and display in CT and others BUG about display
    uint8_t CtModeState : 1;                                                    //APW : Update command CtmodeState and display in CT and others BUG about display
    uint8_t reserved_0 : 1;                                                     //APW : Update command CtmodeState and display in CT and others BUG about display

    // 7
    tuDisplayFunctions funcs; // 7 bytes

    // 14
    uint8_t mbCommand; // teMainBoardCommand
    uint8_t dpEvent;   // teDisplayEvent

    // 16
    tsDisplayInfo displayInfo; // 3 bytes
    // 19
    tsDisplayInfo displayInfo2; // 3 bytes
    // 22
    tuDisplayFunctions2 funcs2; // 4 bytes
    // 26

} tsDisplayPacket;

/// @brief  Device specific parameters
typedef struct
{
    tsTarget target;                                     ///< Device target information on communication device
    tTicket ticket;                                      ///< Ticket that will be used for spi communication
    tsDisplayPacket data;                                ///< display data
} tsDisplayParams;

/// @brief  Device specific constants
typedef struct
{
    const tsDevCom *spi; ///< the spi device that will be used in communications
    tsProcess *process;  ///< Process that handles the jobs for this device
    uint8_t period;      ///< the period between two communication operations with display.
                         ///< one communication operation includes sending & receiveing with 5 ms interval
} tsDisplayConsts;

/** @brief  Create a devCpx
 *  @param  _name   Name of devCpx display object
 *  @param  _period The period between two communication operations with display in ms
 *                  one communication operation includes sending & receiveing with 5 ms interval
 */ 
#define DEV_CPX_DISPLAY_SPI_CREATE(_name, _spi, _period)                                                                \
    tsProcess _name##Process;                                                                                           \
    tsDisplayParams _name##Params;                                                                                                         \
    const tsDisplayConsts _name##Consts =                                                                            \
        {                                                                                                            \
            .spi = &_spi,                                                                                            \
            .process = &_name##Process,                                                                              \
            .period = _period,                                                                                       \
    };                                                                                                               \
    PROCESS_CREATE(_name##Process, displayProcInit, displayProcDeinit, PROCESS_NONE, &_name##Params, &_name##Consts) \
    DEV_CPX_CREATE(_name, devCpxDisplayFuncs, &_name##Params, &_name##Consts)


/** @brief  Set the data that will be shown on the 7-segments of a cabin
 *  @param  device ptr to display device
 *  @param  cabin the cabin which its 7-segments will show the data, from teDisplayCabins enum
 *  @param  dataType the type of the data that will be shown, from teDisplayCabinDataTypes enum
 *  @param  data the data will be displayed, 
 *               if dataType is number, its value can be in range [-99,99], 
 *               if dataType is special (character), its value can be from enumDisplayCabinSpecialData enum
 */
INTERFACE void devDspSetCabinData(const tsDevCpx *device, teDisplayCabins cabin, teDisplayCabinDataTypes dataType, int8_t data);

/** @brief  Sends a command to the display
 *  @param  device ptr to display device
 *  @param  command the command to perform, from teMainBoardCommand enum
 */
INTERFACE void devDspSendCommand(const tsDevCpx *device, teMainBoardCommand command);

/** @brief  Returns an event occurred on the display
 *  @param  device ptr to display device
 *  @return  the occurred event from teDisplayEvent enum, or eDisplayEventNone if no new event is occurred
 */
INTERFACE teDisplayEvent devDspGetEvent(const tsDevCpx *device);

/** @brief  Returns info about the connected display (ver, rev, id) combined in tsDisplayInfo struvt
 *  @param  device ptr to display device
 */
INTERFACE tsDisplayInfo devDspGetInfo(const tsDevCpx *device);

/** @brief  Returns the ID of the connected display
 *  @param  device ptr to display device
 */
INTERFACE uint8_t devDspGetId(const tsDevCpx *device);

/** @brief  Changes the state of a specific led on the display
 *  @param  device ptr to display device
 *  @param  led the led to be controlled, from teDisplayLeds enum
 *  @param  state the new state of the led, from teDisplayLedState enum
 */
INTERFACE void devDspSetLedState(const tsDevCpx *device, teDisplayLeds led, teDisplayLedState state);

/** @brief  Returns the current state of a led on the display
 *  @param  device ptr to display device
 *  @param  led the led to get its state, from teDisplayLeds enum
 *  @return  the current state of the led, from teDisplayLedState enum
 */
INTERFACE teDisplayLedState devDspGetLedState(const tsDevCpx *device, teDisplayLeds led);

/** @brief  Set the dimming level of the display
 *  @param  device ptr to display device
 *  @param  level dimming level, from teDisplayDimLevels enum
 */
INTERFACE void devDspSetDimLevel(const tsDevCpx *device, teDisplayDimLevels level);

/** @brief  Turn off the display ?
 *  @param  device ptr to display device
 */
INTERFACE void devDspSetDisplayOff(const tsDevCpx *device);


INTERFACE void devDspSyncPacket(const tsDevCpx *device, tsDisplayPacket *packet);

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_DISPLAY_H
