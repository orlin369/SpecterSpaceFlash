// IntelHexParser.h

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

#ifndef _INTELHEXPARSER_h
#define _INTELHEXPARSER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ApplicationConfiguration.h"

#include "DebugPort.h"

class IntelHexParserClass
{
public:
	IntelHexParserClass();
	void ParseLine(byte* data);
	byte* GetMemoryPage();
	byte* GetLoadAddress();
	bool IsPageReady();

private:
	int _address = 0;
	int _length = 0;
	int _nextAddress = 0;
	int _memIdx = 0;
	int _recordType = 0;
	byte _memoryPage[128];
	byte _loadAddress[2];
	bool _pageReady = false;
	bool _firstRun = true;

	int GetAddress(byte* hexline);
	int GetLength(byte* hexline);
	int GetRecordType(byte* hexline);
	byte* GetData(byte* hexline, int len);
	void GetLoadAddress(byte* hexline);
	void EndOfFile();
};

extern IntelHexParserClass IntelHexParser;

#endif

