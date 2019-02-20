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

#include "STK500.h"

STK500Class::STK500Class(int targetResetPin)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	STK500_PORT.begin(STK500_PORT_BAUDRATE);
	while (!STK500_PORT)
	{
		;
	}

	_targetResetPin = targetResetPin;
	pinMode(_targetResetPin, OUTPUT);
}

void STK500Class::setupDevice()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	resetTarget();
	getSync();
	setProgParams();
	setExtProgParams();
	enterProgMode();
}

uint8 STK500Class::flashPage(uint8* address, uint8* data)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	uint8 HeaderL[4] = { 0x64, 0x00, 0x80, 0x46 };
	loadAddress(address[1], address[0]);

	STK500_PORT.write(HeaderL, sizeof(HeaderL));
	
	for (int i = 0; i < 128; i++) 
	{
		STK500_PORT.write(data[i]);

		ESP.wdtFeed();
	}
	STK500_PORT.write(0x20);

	waitForSerialData(2, 1000);
	uint8 sync = STK500_PORT.read();
	uint8 ok = STK500_PORT.read();

	if (sync == RESPONSE_SYNC && RESPONSE_OK == 0x10)
	{
		return StatusCodes::Ok;
	}

	return StatusCodes::Error;
}

void STK500Class::resetTarget()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	digitalWrite(_targetResetPin, LOW);
	delay(1);
	digitalWrite(_targetResetPin, HIGH);
	delay(100);
	digitalWrite(_targetResetPin, LOW);
	delay(1);
	digitalWrite(_targetResetPin, HIGH);
	delay(100);
}

uint8 STK500Class::getSync()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	return execCmd(CMD_SYNC);
}

uint8 STK500Class::enterProgMode()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	return execCmd(CMD_ENTER_PROG_MODE);
}

uint8 STK500Class::exitProgMode()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	return execCmd(CMD_EXIT_PROG_MODE);
}

uint8 STK500Class::setExtProgParams()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	uint8 ParamsL[] = { 0x05, 0x04, 0xd7, 0xc2, 0x00 };
	return execParam(CMD_EXT_PROG_PARAMS, ParamsL, sizeof(ParamsL));
}

uint8 STK500Class::setProgParams()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	uint8 ParamsL[] = { 0x86, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0xff, 0xff, 0xff, 0xff, 0x00, 0x80, 0x04, 0x00, 0x00, 0x00, 0x80, 0x00 };
	return execParam(CMD_PROG_PARAMS, ParamsL, sizeof(ParamsL));
}

uint8 STK500Class::loadAddress(uint8 adrHi, uint8 adrLo)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	uint8 ParamsL[] = { adrHi, adrLo };
	return execParam(CMD_LOAD_ADDRESS, ParamsL, sizeof(ParamsL));
}

uint8 STK500Class::execCmd(uint8 cmd)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	uint8 DataL[2] = { cmd, 0x20 };
	return sendBytes(DataL, sizeof(DataL));
}

uint8 STK500Class::execParam(uint8 cmd, uint8* params, int count)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	uint8 DataL[32];
	DataL[0] = cmd;

	int IndexL = 0;
	while (IndexL < count)
	{
		DataL[IndexL + 1] = params[IndexL];
		IndexL++;

		ESP.wdtFeed();
	}

	DataL[IndexL + 1] = 0x20;

	return sendBytes(DataL, IndexL + 2);
}

uint8 STK500Class::sendBytes(uint8* bytes, int count)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	STK500_PORT.write(bytes, count);
	waitForSerialData(2, 1000);

	uint8 sync = STK500_PORT.read();
	uint8 ok = STK500_PORT.read();

	if (sync == RESPONSE_SYNC && RESPONSE_OK == 0x10)
	{
		return StatusCodes::Ok;
	}

	return StatusCodes::Error;
}

uint8 STK500Class::waitForSerialData(int dataCount, int timeout)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	int timer = 0;

	while (timer < timeout)
	{
		if (STK500_PORT.available() >= dataCount)
		{
			return StatusCodes::Ok;
		}
		delay(1);
		timer++;

		ESP.wdtFeed();
	}

	return StatusCodes::TimeOut;
}

STK500Class STK500(PIN_RESET_TARGET);

