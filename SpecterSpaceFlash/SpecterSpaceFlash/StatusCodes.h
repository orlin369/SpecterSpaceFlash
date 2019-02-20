// StatusCodes.h

#ifndef _STATUSCODES_h
#define _STATUSCODES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/** @brief Status codes bytes. */
enum StatusCodes : uint8
{
	Ok = 1U, ///< When everything is OK.
	Error, ///< When error occurred.
	Busy, ///< When busy in other operation.
	TimeOut, ///< Then time for the operation has timed out.
};

#endif

