#ifndef UI_PROCEDURES_RAM_H
#define UI_PROCEDURES_RAM_H

typedef struct
{
	unsigned char errorsIndex;
	unsigned char errorShowState;
} tsUiErrors;

/************************************/

typedef enum
{
	_UI_PasswordState_Enter,
} teUiPasswordStates;

typedef struct
{
	teUiPasswordStates state;
	unsigned char stayTimer;

	unsigned short passwordEntered;
	unsigned char passwordKeyCount;
	unsigned char passwordKey;
} tsUiPasswordData;

/************************************/

typedef enum
{
	_UI_SabbathState_Active,
} teUiSabbathState;

/************************************/

#define _MAX_SLIDES_COUNT 5


typedef struct
{
	unsigned char data;
	teDisplayCabinDataTypes dataType;
} stSlide;

typedef struct
{
	stSlide slide[_MAX_SLIDES_COUNT];
	unsigned char slideIndex;
	unsigned char slideNumber;
} tsSlides;

/************************************/

typedef enum
{
	_en_reset_default = 0,
	_en_reset_send_1,
	_en_reset_wait_2,
	_en_reset_wait_4,
	_en_reset_cancel = 60
} enWifiResetStatus;

typedef struct
{
	enWifiResetStatus resetStatus;
	unsigned char notConnectedResetFlag;
} tsUiWifiData;

#endif
