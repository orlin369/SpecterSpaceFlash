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

#include "IntelHexParser.h"

IntelHexParserClass::IntelHexParserClass()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	_loadAddress[0] = 0x00;
	_loadAddress[1] = 0x00;
}

void IntelHexParserClass::ParseLine(byte* hexline)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	_recordType = GetRecordType(hexline);

	if (_recordType == 0) {
		_address = GetAddress(hexline);
		_length = GetLength(hexline);


		GetData(hexline, _length);

		if (_memIdx == 128) {

			if (!_firstRun) {
				_loadAddress[1] += 0x40;
				if (_loadAddress[1] == 0) {
					_loadAddress[0] += 0x1;
				}
			}
			_firstRun = false;
			_pageReady = true;
			_memIdx = 0;
		}

		_nextAddress = _address + _length;

	}

	if (_recordType == 1) {
		EndOfFile();
		_pageReady = true;
	}

}

bool IntelHexParserClass::IsPageReady()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	return _pageReady;
}

byte* IntelHexParserClass::GetMemoryPage()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	_pageReady = false;
	return _memoryPage;
}

byte* IntelHexParserClass::GetLoadAddress()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
		
	return _loadAddress;
}

void IntelHexParserClass::GetLoadAddress(byte* hexline)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	char buff[3];
	buff[2] = '\0';

	buff[0] = hexline[3];
	buff[1] = hexline[4];
	_loadAddress[0] = strtol(buff, 0, 16);
	buff[0] = hexline[5];
	buff[1] = hexline[6];
	_loadAddress[1] = strtol(buff, 0, 16);
}

byte* IntelHexParserClass::GetData(byte* hexline, int len)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	int start = 9;
	int end = (len * 2) + start;
	char buff[3];
	buff[2] = '\0';

	for (int x = start; x < end; x = x + 2) {
		buff[0] = hexline[x];
		buff[1] = hexline[x + 1];
		_memoryPage[_memIdx] = strtol(buff, 0, 16);
		_memIdx++;
	}

}

void IntelHexParserClass::EndOfFile()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	_loadAddress[1] += 0x40;
	if (_loadAddress[1] == 0) {
		_loadAddress[0] += 0x1;
	}

	while (_memIdx < 128) {
		_memoryPage[_memIdx] = 0xFF;
		_memIdx++;
	}

}

int IntelHexParserClass::GetAddress(byte* hexline)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	char buff[5];
	buff[0] = hexline[3];
	buff[1] = hexline[4];
	buff[2] = hexline[5];
	buff[3] = hexline[6];
	buff[4] = '\0';

	return strtol(buff, 0, 16);
}

int IntelHexParserClass::GetLength(byte* hexline)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	char buff[3];
	buff[0] = hexline[1];
	buff[1] = hexline[2];
	buff[2] = '\0';

	return strtol(buff, 0, 16);
}

int IntelHexParserClass::GetRecordType(byte* hexline)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	char buff[3];
	buff[0] = hexline[7];
	buff[1] = hexline[8];
	buff[2] = '\0';

	return strtol(buff, 0, 16);
}

IntelHexParserClass IntelHexParser;

