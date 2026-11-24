# RCoS+ 2.0

[Reusable COnnected Software platform + security](https://rcossdk.com/) <!-- @ref RCoS " More..." -->

## How To INSTALL

1. Place "rcos" folder under your project's root folder,
2. Set include folder compile option to also have "rcos" folder,
3. Run _rcos_config.vbs script and configure "rcos.h" file,
4. Follow the created "rcos.c" and "main.c" example for a setup, initialize and start rcos

## Example "main.c"

~~~~c
#include "rcos.h" // RCoS+ main configuration

CORE_EVENTQUEUE_SIZE(...)
CORE_TICK_DEFAULT(...)
CORE_WDT_DEFAULT(...)

#include "dev/gpio.h"
DEV_IO_GPIO_CREATE(gpioDevice, ...)

#include "dev/uart.h"
DEV_COM_UART_CREATE(comUart, ...)

#include "dev/w25q.h"
DEV_MEM_W25Q_CREATE(memExtFlash, ...)

#include "dev/matrixout.h"
DEV_CPX_MATRIXOUT_CREATE(cpxMatrix, ...)

#include "mw/uartecho.h"
PROCESS_UARTECHO_CREATE(processUartEcho, ...)

int main(void)
{
    platformInit();
    coreInit();

    devIoInit(&gpioDevice, NULL);
    devComInit(&comUart);
    devMemInit(&memExtFlash);
    devCpxInit(&cpxMatrix, NULL);

    processStart(&processUartEcho);

    coreRun();
}
~~~~
