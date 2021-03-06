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

/** @brief Constructor.
 *  @param int targetResetPin, Target RESET GPIO.
 *  @return Void.
 */
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

/** @brief Prepare the target.
 *  @return Void.
 */
void STK500Class::prepareTarget()
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

/** @brief Flash page on specified address.
 *  @param uint8* address, Address of the page.
 *  @param uint8* data, Data for the page.
 *  @return Void.
 */
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

#if defined(ARDUINO_ARCH_ESP8266)
		ESP.wdtFeed();
#endif
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

/** @brief Reset the target.
 *  @return Void.
 */
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

/** @brief Get the sync information.
 *  @return uint8, State of the communication.
 *  @see StatusCodes.h
 */
uint8 STK500Class::getSync()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	return execCmd(CMD_SYNC);
}

/** @brief Enter the target in to programming mode.
 *  @return uint8, State of the communication.
 *  @see StatusCodes.h
 */
uint8 STK500Class::enterProgMode()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	return execCmd(CMD_ENTER_PROG_MODE);
}

/** @brief Escape the target in to programming mode.
 *  @return uint8, State of the communication.
 *  @see StatusCodes.h
 */
uint8 STK500Class::exitProgMode()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	return execCmd(CMD_EXIT_PROG_MODE);
}

/** @brief Set external programming parametters.
 *  @return uint8, State of the communication.
 *  @see StatusCodes.h
 */
uint8 STK500Class::setExtProgParams()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	uint8 ParamsL[5] = { 0x05, 0x04, 0xd7, 0xc2, 0x00 };
	return execParam(CMD_EXT_PROG_PARAMS, ParamsL, sizeof(ParamsL));
}

/** @brief Load address.
 *  @param uint8 adrHi, High byte of the address.
 *  @param uint8 adrLo, Low byte of the address.
 *  @return uint8, State of the communication.
 *  @see StatusCodes.h
 */
uint8 STK500Class::loadAddress(uint8 adrHi, uint8 adrLo)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	uint8 ParamsL[] = { adrHi, adrLo };
	return execParam(CMD_LOAD_ADDRESS, ParamsL, sizeof(ParamsL));
}


/** @brief Set programming parametters.
 *  @return uint8, State of the communication.
 *  @see StatusCodes.h
 */
uint8 STK500Class::setProgParams()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	uint8 ParamsL[] = { 0x86, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0xff, 0xff, 0xff, 0xff, 0x00, 0x80, 0x04, 0x00, 0x00, 0x00, 0x80, 0x00 };
	return execParam(CMD_PROG_PARAMS, ParamsL, sizeof(ParamsL));
}


/** @brief Execute command.
 *  @param uint8 cmd, Command.
 *  @return uint8, State of the communication.
 *  @see StatusCodes.h
 */
uint8 STK500Class::execCmd(uint8 cmd)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	uint8 DataL[2] = { cmd, 0x20 };
	return sendBytes(DataL, sizeof(DataL));
}

/** @brief Execute parameter.
 *  @param uint8 cmd, Command.
 *  @param uint8* params, Parameters.
 *  @param uint8 len, Length of the parameters.
 *  @return uint8, State of the communication.
 *  @see StatusCodes.h
 */
uint8 STK500Class::execParam(uint8 cmd, uint8* params, int len)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	uint8 DataL[32];
	DataL[0] = cmd;

	int IndexL = 0;
	while (IndexL < len)
	{
		DataL[IndexL + 1] = params[IndexL];
		IndexL++;

#if defined(ARDUINO_ARCH_ESP8266)
		ESP.wdtFeed();
#endif
	}

	DataL[IndexL + 1] = 0x20;

	return sendBytes(DataL, IndexL + 2);
}

/** @brief Execute parameter.
 *  @param uint8 data*, Data.
 *  @param uint8 len, Length of the data.
 *  @return uint8, State of the communication.
 *  @see StatusCodes.h
 */
uint8 STK500Class::sendBytes(uint8* data, int len)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	STK500_PORT.write(data, len);
	waitForSerialData(2, 1000);

	uint8 sync = STK500_PORT.read();
	uint8 ok = STK500_PORT.read();

	if (sync == RESPONSE_SYNC && RESPONSE_OK == 0x10)
	{
		return StatusCodes::Ok;
	}

	return StatusCodes::Error;
}

/** @brief Execute parameter.
 *  @param int len, Length of the data.
 *  @param int timeout, Timeout to recieve the data.
 *  @return uint8, State of the communication.
 *  @see StatusCodes.h
 */
uint8 STK500Class::waitForSerialData(int len, int timeout)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	int timer = 0;

	while (timer < timeout)
	{
		if (STK500_PORT.available() >= len)
		{
			return StatusCodes::Ok;
		}
		delay(1);
		timer++;

#if defined(ARDUINO_ARCH_ESP8266)
		ESP.wdtFeed();
#endif
	}

	return StatusCodes::TimeOut;
}

/* @brief Singelton STK500 instance. */
STK500Class STK500(PIN_RESET_TARGET);

