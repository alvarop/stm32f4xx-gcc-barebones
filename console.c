#include <stdio.h>
#include <string.h>
#include "console.h"
#include "fifo.h"

fifo_t usbRxFifo;

static uint8_t cmdBuff[64];
static uint8_t printBuff[1024];
static uint32_t argc;
static char* argv[8];

void consoleProcess() {
	uint32_t inBytes = fifoSize(&usbRxFifo);
	if(inBytes > 0) {
		uint32_t newLine = 0;
		for(int32_t index = 0; index < inBytes; index++){
			if((fifoPeek(&usbRxFifo, index) == '\n') || (fifoPeek(&usbRxFifo, index) == '\r')) {
				newLine = index + 1;
				break;
			}
		}

		if(newLine > sizeof(cmdBuff)) {
			newLine = sizeof(cmdBuff) - 1;
		}

		if(newLine) {
			uint8_t *pBuf = cmdBuff;
			while(newLine--){
				*pBuf++ = fifoPop(&usbRxFifo);
			}

			// If it's an \r\n combination, discard the second one
			if((fifoPeek(&usbRxFifo, 0) == '\n') || (fifoPeek(&usbRxFifo, 0) == '\r')) {
				fifoPop(&usbRxFifo);
			}

			*(pBuf - 1) = 0; // String terminator

			argc = 0;

			// Get command
			argv[argc] = strtok(cmdBuff, " ");

			// Get arguments (if any)
			while ((argv[argc] != NULL) && (argc < sizeof(argv))){
				argc++;
				argv[argc] = strtok(NULL, " ");
			}

			if(argc > 0) {
				snprintf(printBuff, sizeof(printBuff), "argc = %d\n", argc);
				puts(printBuff);

				snprintf(printBuff, sizeof(printBuff), "cmd = \"%s\"\n", argv[0]);
				puts(printBuff);

				for(uint8_t arg = 1; arg < argc; arg++) {
					snprintf(printBuff, sizeof(printBuff), "arg %d = \"%s\"\n", arg, argv[arg]);
					puts(printBuff);				
				}
			}
		}
	}
}
