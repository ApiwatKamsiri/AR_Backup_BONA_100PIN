/** @file       morse.c
 *  @brief      C file of MORSE device
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       1.10.2019
 */
#define FILE_MORSE_C

#include "morse.h"

/**
 *  @addtogroup DEV_MORSE
 *  @{
 */

static DEV_COM_FUNC_INIT(morseInit);
static DEV_COM_FUNC_DEINIT(morseDeinit);
static DEV_COM_FUNC_OPEN(morseOpen);
static DEV_COM_FUNC_CLOSE(morseClose);
static DEV_COM_FUNC_SEND(morseSend);

/// @brief  Structure that defines the functions for morse
const tsDevComFuncs devComMorseFuncs =
    {
        morseInit,
        morseDeinit,
        morseOpen,
        morseClose,
        morseSend,
        NULL,
        NULL,
        NULL,
};

/// @brief  Structure to define morse code symbols
typedef struct
{
    uint8_t symbol; ///< Symbol definition
    uint8_t pulse;  ///< Pulse width for this symbol
    uint8_t total;  ///< Total time spent for this symbol
} tsSymbolConvert;

/// @brief  Morse code symbol list
const tsSymbolConvert morseSymbolArr[] =
    {
        {'\0', 0, 2}, ///< Space between chars  // Pulse(0) + Silence(3) - (previous dot/dash silence)
        {' ', 0, 4},  ///< Space between words  // Pulse(0) + Silence(7) - (Silence between chars)
        {'.', 1, 2},  ///< Dot                  // Pulse(1) + Silence(1)
        {'-', 3, 4},  ///< Dash                 // Pulse(3) + Silence(1)
};

/** @brief  Send the symbol's pulse width information to pulse device
 *          and return total duration to process next symbol
 *  @param  symbol  Coded symbol, dot, dash or space
 *  @param  pulse   Pulse device to output signal of the symbol
 *  @param  unit    One unit of time for pulse device in milliseconds
 *  @return Duration to wait until processing next symbol
 */
uint32_t morseSendSymbol(uint8_t symbol, const tsDevIo *pulse, uint32_t unit)
{
    uint8_t idx = 0;

    for (idx = 0; idx < ARRAY_SIZE(morseSymbolArr); idx++)
    {
        if (morseSymbolArr[idx].symbol == symbol)
        {
            if (morseSymbolArr[idx].pulse)
            {
                devIoPut(pulse, unit * morseSymbolArr[idx].pulse);
            }
            return (unit * morseSymbolArr[idx].total);
        }
    }

    return 0; // This is not a known symbol
}

/// @brief  Morse Code alphabet
const char *morseAlphabet[] =
    {
        "",  // Empty
        " ", // Space

        "-----", // 0 // idx = 2
        ".----", // 1
        "..---", // 2
        "...--", // 3
        "....-", // 4
        ".....", // 5
        "-....", // 6
        "--...", // 7
        "---..", // 8
        "----.", // 9

        ".-",   // A // idx = 12
        "-...", // B
        "-.-.", // C
        "-..",  // D
        ".",    // E
        "..-.", // F
        "--.",  // G
        "....", // H
        "..",   // I
        ".---", // J
        "-.-",  // K
        ".-..", // L
        "--",   // M
        "-.",   // N
        "---",  // O
        ".--.", // P
        "--.-", // Q
        ".-.",  // R
        "...",  // S
        "-",    // T
        "..-",  // U
        "...-", // V
        ".--",  // W
        "-..-", // X
        "-.--", // Y
        "--..", // Z
};

/** @brief  Convert a ASCII character to corresponding index of morseAlphabet
 *  @param  ascii   ASCII character code
 *  @return Index value from morseAlphabet
 */
uint8_t morseAscii2Idx(uint8_t ascii)
{
    if (ascii == ' ')
    {
        return 1;
    }
    if (ascii >= '0')
    {
        if (ascii <= '9')
        {
            return ascii - '0' + 2;
        }
        if (ascii >= 'A')
        {
            if (ascii <= 'Z')
            {
                return ascii - 'A' + 12;
            }
            if (ascii >= 'a')
            {
                if (ascii <= 'z')
                {
                    return ascii - 'a' + 12;
                }
            }
        }
    }

    return 0; // Non-convertable characters are returned as 0
}

/** @brief  Find the next symbol for pointed device
 *  @param  device Main device pointer
 */
void morseNextSymbol(const tsDevCom *device)
{
    tsMorseParams *params       = device->parameters;
    const tsMorseConsts *consts = device->constants;

    if (params->symbol == NULL)
    {
        // Currently there's no symbol being transmitted
        if (FIFO_LOAD(consts->fifo))
        {
            // Get a new character from fifo and show its first symbol as next symbol
            uint8_t nextCharIdx;
            FIFO_POP(consts->fifo, &nextCharIdx, sizeof(nextCharIdx));
            params->symbol = morseAlphabet[nextCharIdx];
        }
        else
        {
            // No more characters to output
            params->symbol = NULL;
        }
    }
    else
    {
        if (*params->symbol == '\0')
        {
            // Last character is finished
            if (FIFO_LOAD(consts->fifo))
            {
                // Get a new character from fifo and show its first symbol as next symbol
                uint8_t nextCharIdx;
                FIFO_POP(consts->fifo, &nextCharIdx, sizeof(nextCharIdx));
                params->symbol = morseAlphabet[nextCharIdx];
            }
            else
            {
                // No more characters to output
                params->symbol = NULL;
            }
        }
        else
        {
            // Jump to next symbol
            params->symbol++;
        }
    }
}

TIMER_CALLBACK_FUNC(morseProcess)
{
    const tsDevCom *device      = parameter;
    tsMorseParams *params       = device->parameters;
    const tsMorseConsts *consts = device->constants;
    tsDevComSystem *sys         = device->sys;

    morseNextSymbol(device); // Finds the next symbol for processing

    if (params->symbol == NULL)
    {
        // Last call
        TICKET_REMOVE(sys->tvm);
        sys->closed = sys->opened;
        return 0;
    }
    else
    {
        uint32_t newTimeout;
        // Send symbol and update timer accordingly for next symbol
        newTimeout = morseSendSymbol(*params->symbol, consts->pulse, consts->unit);
        if (newTimeout == 0)
        {
            // Last call
            TICKET_REMOVE(sys->tvm);
            sys->closed = sys->opened;
        }
        return newTimeout;
    }
}

/// @brief  Init function for a morse
static DEV_COM_FUNC_INIT(morseInit)
{
    const tsMorseConsts *consts = device->constants;
    ADD_RCOS_VERSION("morse", "2.0.0")

    devIoInit(consts->pulse, NULL);
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a morse
static DEV_COM_FUNC_DEINIT(morseDeinit)
{
    tsMorseParams *params       = device->parameters;
    const tsMorseConsts *consts = device->constants;

    timerCallBackStop(&params->tcb);
    FIFO_FLUSH(consts->fifo);
    device->sys->initialized = 0;

    return EXIT_SUCCESS;
}

/// @brief  Open function for a morse
static DEV_COM_FUNC_OPEN(morseOpen)
{
    tsDevComSystem *sys = device->sys;

    UNUSED(target);

    if (!sys->initialized)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/// @brief  Close function for a morse
static DEV_COM_FUNC_CLOSE(morseClose)
{
    tsDevComSystem *sys = device->sys;

    UNUSED(target);

    if (!sys->initialized)
    {
        return EXIT_FAILURE;
    }

    TICKET_REMOVE(sys->tvm);

    return EXIT_SUCCESS;
}

/// @brief  Send function for a morse
static DEV_COM_FUNC_SEND(morseSend)
{
    tsMorseParams *params       = device->parameters;
    const tsMorseConsts *consts = device->constants;
    tsDevComSystem *sys         = device->sys;

    if (!sys->initialized)
    {
        return 0;
    }

    if (!length)
    {
        // Return data count in transmit buffer
        length = FIFO_LOAD(consts->fifo);
    }
    else // if (!length)
    {
        if (txb == NULL)
        {
            // No operation
        }
        else // if (txb == NULL)
        {
            // Place data into transmit buffer
            const uint8_t *chars = txb;

            if (FIFO_FREE(consts->fifo) >= length)
            {
                for (; length; length--)
                {
                    uint8_t charIdx = morseAscii2Idx(*chars++);
                    FIFO_PUSH(consts->fifo, &charIdx, sizeof(charIdx));
                }
                sys->closed = 0;

                if (params->tcb._cnt == 0)
                {
                    uint32_t duration;

                    morseNextSymbol(device);
                    duration = morseSendSymbol(*params->symbol, consts->pulse, consts->unit);
                    if (duration != 0)
                    {
                        timerCallBackStart(&params->tcb, duration);
                    }
                }
            }
            else
            {
                length = 0;
            }
        } // else // if (txb == NULL)
    }     // else // if (!length)

    return length;
}

/** @} */
