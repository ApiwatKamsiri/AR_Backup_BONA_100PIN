/** @file       cli_cmds.c
 *  @brief      CLI_CMDS middleware program file
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       27/05/2020
 */
#define FILE_CLI_CMDS_C

#include "cli_cmds.h"

#define DEBUG_FILE_NAME "clicmd"

// #define DEBUG_PORT_
// DEBUG_START(0)

/** @addtogroup CLI_CMDS
 *  @{
 */

CMD_FUNC(cmdCls, "cls", "Clear screen", "")
{
    CMD_BEGIN();

    CMD_PRINT(TERMINAL_CURSOR_POS(0, 0) TERMINAL_ERASE_DISPLAY);

    CMD_END();
}

CMD_FUNC(cmdEcho, "echo", "Echo back the input text", "")
{
    CMD_BEGIN();

    for (int idx = 1; idx < CMD_ARGC(); idx++)
    {
        CMD_PRINT("%s ", CMD_ARGV()[idx]);
    }

    CMD_END();
}

static const char *cpadKeys[] =
    {
        "home",
        "insert",
        "delete",
        "end",
        "page-up",
        "page-down",
};

CMD_FUNC(cmdKeyTest, "keytest", "Test the keyboard inputs", "")
{
    char readByte = ASCII_NUL;

    CMD_BEGIN();

    CMD_PRINT(TERMINAL_CURSOR_POS(0, 0) TERMINAL_ERASE_DISPLAY);

    for (;;)
    {
        PT_WAIT_UNTIL_W_PERIOD(CMD_RECV(&readByte, sizeof(readByte)), 100);
        if (ASCII_ESC == readByte)
        {
            CMD_ESC_SEQ();

            CMD_ESC_IF_APAD_UP()
            {
                // up
                CMD_PRINT("up\r\n");
            }
            else CMD_ESC_IF_APAD_DWN()
            {
                // down
                CMD_PRINT("down\r\n");
            }
            else CMD_ESC_IF_APAD_FWD()
            {
                // forward
                CMD_PRINT("right\r\n");
            }
            else CMD_ESC_IF_APAD_BWD()
            {
                // backward
                CMD_PRINT("left\r\n");
            }
            else CMD_ESC_IF_CPAD()
            {
                if (params->escVal[0] <= ARRAY_SIZE(cpadKeys))
                {
                    // CPAD
                    CMD_PRINT("%s\r\n", cpadKeys[params->escVal[0] - 1]);
                }
                else
                {
                    // Function keys
                    if (params->escVal[0] > 22) // fix 1
                    {
                        params->escVal[0] -= 1;
                    }
                    if (params->escVal[0] > 16) // fix 1
                    {
                        params->escVal[0] -= 1;
                    }
                    CMD_PRINT("F%i\r\n", params->escVal[0] - 10);
                }
            }
            else
            {
                CMD_PRINT("%i:%i:%i:%c\r\n", params->escVal[0], params->escVal[1], params->escVal[2], params->escOpt);
            }
        }
        else if ((ASCII_CR == readByte) || (ASCII_LF == readByte))
        {
            break;
        }
    }

    CMD_PRINT(TERMINAL_CURSOR_POS(0, 0) TERMINAL_ERASE_DISPLAY);

    CMD_END();
}

/** @} */
