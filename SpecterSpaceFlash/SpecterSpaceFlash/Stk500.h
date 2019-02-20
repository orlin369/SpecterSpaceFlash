// STK500.h

/*

Copyright (c) [2019] [Orlin Dimitrov]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef _STK500_h
#define _STK500_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ApplicationConfiguration.h"

#include "DebugPort.h"

#include "StatusCodes.h"

#define CMD_SYNC 0x30
#define CMD_ENTER_PROG_MODE 0x50
#define CMD_EXIT_PROG_MODE 0x51
#define CMD_EXT_PROG_PARAMS 0x45
#define CMD_PROG_PARAMS 0x42
#define CMD_LOAD_ADDRESS 0x55
#define RESPONSE_OK 0x10
#define RESPONSE_SYNC 0x14

class STK500Class
{
public:
	STK500Class(int targetResetPin);
	void resetTarget();
	uint8 getSync();
	uint8 enterProgMode();
	uint8 exitProgMode();
	uint8 loadAddress(uint8 adrHi, uint8 adrLo);
	uint8 setProgParams();
	uint8 setExtProgParams();

	void setupDevice();
	uint8 flashPage(uint8* loadAddress, uint8* data);

private:
	uint8 execCmd(uint8 cmd);
	uint8 execParam(uint8 cmd, uint8* params, int count);
	uint8 sendBytes(uint8* bytes, int count);
	uint8 waitForSerialData(int dataCount, int timeout);
	int getFlashPageCount(uint8 flashData[][131]);

	int _targetResetPin;
};

extern STK500Class STK500(PIN_RESET_TARGET);

#endif

