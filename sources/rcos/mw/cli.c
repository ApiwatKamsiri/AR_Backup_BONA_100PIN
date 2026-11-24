/** @file       cli.c
 *  @brief      CLI middleware program file
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       20/05/2020
 */
#define FILE_CLI_C

#include "cli.h"
#include <ctype.h>
#include <string.h>

#define DEBUG_FILE_NAME "cli"

// #define DEBUG_PORT_
// DEBUG_START(0)

/**
 *  @addtogroup CLI
 *  @{
 */

#define CLI_PROMPT "rcos+> "

CMD_FUNC(cmdHelp, "help", "Print short description strings of every command", "")
{
    CMD_BEGIN();

    for (params->escCnt = 0; params->escCnt < consts->cmdCnt; params->escCnt++)
    {
        PT_WAIT_UNTIL(CMD_PRINT("%-16s%s\r\n", consts->cmdList[params->escCnt]->command, consts->cmdList[params->escCnt]->desc));
    }

    CMD_END();
}

PT_THREAD(cliEscSeqParser)
{
    CMD_BEGIN();

    params->escCnt    = 0;
    params->escVal[0] = 0;
    params->escOpt    = ASCII_NUL;

    PT_WAIT_UNTIL(CMD_RECV(&params->readByte, sizeof(params->readByte)));
    if ('[' != params->readByte)
    {
        // Escape sequence failed
        PT_EXIT();
    }

    for (;;)
    {
        PT_WAIT_UNTIL(CMD_RECV(&params->readByte, sizeof(params->readByte)));
        if (isdigit(params->readByte))
        {
            params->escVal[params->escCnt] *= 10;
            params->escVal[params->escCnt] += params->readByte - '0';
        }
        else if (';' == params->readByte) // delimiter
        {
            if (params->escCnt < (ARRAY_SIZE(params->escVal) - 1))
            {
                ++params->escCnt;
                params->escVal[params->escCnt] = 0;
            }
        }
        else if ('O' == params->readByte)
        {
            // This must be followed by another character for function keys
        }
        else if (isalpha(params->readByte))
        {
            ++params->escCnt; // this is not an index anymore
            params->escOpt = params->readByte;
            PT_EXIT();
        }
        else if ('~' == params->readByte)
        {
            ++params->escCnt; // this is not an index anymore
            params->escOpt = params->readByte;
            PT_EXIT();
        }
        else // There should not be something else
        {
            params->escCnt = 0;
            params->escOpt = ASCII_NUL;
            PT_EXIT();
        }
    }

    CMD_END();
}

static PT_THREAD(cliMainThread)
{
    tsCliParams *params       = process->parameters;
    const tsCliConsts *consts = process->constants;
    teBool echo               = FALSE;

    PT_BEGIN();

    devComInit(consts->uart);
    params->uartTicket = devComTicketGet(consts->uart);
    PT_WAIT_UNTIL_W_PERIOD(devComTicketValid(consts->uart, params->uartTicket), 10);
    devComOpen(consts->uart, consts->target);
    devComPrint(consts->uart, "\r\n" CLI_PROMPT);
    process->initialized = 1;

    for (;;)
    {
        echo = FALSE;

        PT_WAIT_UNTIL_W_PERIOD(devComReceive(consts->uart, &params->readByte, sizeof(params->readByte)), 10);

        // CLI
        if (params->readByte < ' ') // Control Character Parser
        {
            if (ASCII_ESC == params->readByte) // Escape sequence
            {
                CMD_ESC_SEQ();
            }
            else if (ASCII_BEL == params->readByte)
            {
                // devIoPut(params->buzzer,
            }
            else if (ASCII_TAB == params->readByte) // Command Completion feature
            {
                // Find command
                for (params->cmdIdx = 0; params->cmdIdx < consts->cmdCnt; params->cmdIdx++)
                {
                    if (strncmp(params->buffer, consts->cmdList[params->cmdIdx]->command, params->bufferIdx) == 0)
                    {
                        CMD_PRINT(TERMINAL_CURSOR_LEFT(%i), (params->bufferIdx - 1));
                        strcpy(params->buffer, consts->cmdList[params->cmdIdx]->command);
                        params->bufferIdx = strlen(params->buffer);
                        CMD_PRINT(params->buffer);
                        break;
                    }
                }
            }
            else if (ASCII_CR == params->readByte) // '\r'
            {
                devComSend(consts->uart, &params->readByte, sizeof(params->readByte)); // echo

                params->readByte = ASCII_LF;
                devComSend(consts->uart, &params->readByte, sizeof(params->readByte));

                // Seperate arguments
                params->argc = 0;
                memset(params->argv, 0, sizeof(params->argv));
                params->buffer[params->bufferIdx] = ASCII_NUL; // nul termination

                for (int idx = 0; idx <= params->bufferIdx; idx++)
                {
                    if (params->buffer[idx] == ' ')
                    {
                        params->buffer[idx] = ASCII_NUL; // seperate each argument with null chars
                    }
                    if (!params->argv[params->argc])
                    {
                        // We're trying to find the start of an argument
                        if (' ' < params->buffer[idx])
                        {
                            params->argv[params->argc] = (char *)&params->buffer[idx];
                        }
                    }
                    else
                    {
                        // We're trying to find the end of the argument
                        if (ASCII_NUL == params->buffer[idx])
                        {
                            if (params->argc < (ARRAY_SIZE(params->argv) - 1))
                            {
                                ++params->argc;
                            }
                        }
                    }
                }

                if (params->argc)
                {
                    // Find command
                    for (params->cmdIdx = 0; params->cmdIdx < consts->cmdCnt; params->cmdIdx++)
                    {
                        if (strncmp(params->buffer, consts->cmdList[params->cmdIdx]->command, strlen(consts->cmdList[params->cmdIdx]->command)) == 0)
                        {
                            PT_SPAWN(&params->cmdThread, consts->cmdList[params->cmdIdx]->function);
                            break;
                        }
                        PT_YIELD();
                    }

                    // Command not found
                    if (params->cmdIdx == consts->cmdCnt)
                    {
                        PT_WAIT_UNTIL(CMD_PRINT("Unknown command: %s", CMD_ARGV()[0]));
                    }
                }

                // Prompt
                PT_WAIT_UNTIL(CMD_PRINT("\r\n" CLI_PROMPT));
                params->bufferIdx = 0;
            }
            else if (ASCII_LF == params->readByte) // '\n'
            {
                echo = TRUE;
            }
            else if (ASCII_BS == params->readByte)
            {
                if (params->bufferIdx > 0)
                {
                    params->buffer[--params->bufferIdx] = ASCII_NUL; // Remove char
                    echo                                = TRUE;
                }
            }
            else
            {
                // Don't process the rest
                devComPrint(consts->uart, "^%c", params->readByte + 'A' - 1);
            }
        }
        else if (ASCII_DEL == params->readByte) // ASCII_DEL character
        {
            if (params->bufferIdx > 0)
            {
                params->buffer[--params->bufferIdx] = ASCII_NUL; // Remove char
                echo                                = TRUE;
            }
        }
        else // Printable characters
        {
            // Store rest of the data as input
            params->buffer[params->bufferIdx] = params->readByte; // store for commands
            if (params->bufferIdx < (sizeof(params->buffer) - 1))
            {
                ++params->bufferIdx;
            }
            else
            {
                uint8_t temp = ASCII_BS;
                devComSend(consts->uart, &temp, sizeof(temp)); // send a backspace char
            }
            echo = TRUE;
        }

        if (TRUE == echo)
        {
            devComSend(consts->uart, &params->readByte, sizeof(params->readByte)); // echo
        }

    } // for (;;)

    devComClose(consts->uart, consts->target);
    process->initialized = 0;

    PT_END();
}

/// @brief  Initialization function of cli
PROCESS_INIT_PROTO(cliInit)
{
    ADD_RCOS_VERSION("cli", "2.0.0")

    threadStart(process, cliMainThread);
}

/// @brief  Deinitialization function of cli
PROCESS_DEINIT_PROTO(cliDeinit)
{
    threadStop(process, cliMainThread);
}

/** @} */
