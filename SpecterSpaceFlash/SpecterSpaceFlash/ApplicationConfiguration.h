// ApplicationConfiguration.h

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

#ifndef _APPLICATIONCONFIGURATION_h
#define _APPLICATIONCONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma region General Configuration

/** @brief Brand name. */
#define DEVICE_BRAND "Specter Space Flash"

/** @brief Firmware version string. */
#define FW_VERSION 1

#define EANBLE_DEBUG_OUT

#define ENABLE_WEB_EDITOR

//#define ENABLE_OTA_ARDUINO

#define ENABLE_WEB_OTA

#define ENABLE_SELF_OTA

#pragma endregion

#pragma region GPIO Map

#define PIN_RESET_TARGET D4

#pragma endregion

#pragma region Debug Port

#ifdef EANBLE_DEBUG_OUT

#define DEBUG_PORT Serial1

#define DEBUG_PORT_BAUDRATE 115200

#endif // EANBLE_DEBUG_OUT

#pragma endregion

#pragma region AP Configuration

/** @brief Default AP password. */
#define DEFAULT_AP_PASSWORD "12345678"

#pragma endregion

#pragma region STA Configuration

/** @brief Default STA SSID. */
#define DEFAULT_STA_SSID "YOUR_WIFI_PASSWORD"

/** @brief Default STA password. */
#define DEFAULT_STA_PASSWORD "YOUR_WIFI_SSID"

#pragma endregion

#pragma region HTTP WEB / Authentication

#define PORT_HTTP 80

/** @brief Default HTTP username. */
#define DEFAULT_HTTP_USERNAME "admin"

/** @brief Default HTTP password. */
#define DEFAULT_HTTP_PASSWORD "admin"

#pragma endregion

#pragma region Config File Path

/* @brief JSON configuration file. */
#define CONFIG_FILE "/config.json"

#pragma endregion

#pragma region OTA Updates

#define SERVER_DOMAIN "http://specter.space.flash.com"

/** @brief Updatest root endpoint. */
#define UPDATE_SERVER_PATH "api/v1/specter_space_flash/fota/update"

/** @brief Version root endpoint. */
#define VERSION_SERVER_PATH "api/v1/specter_space_flash/fota/version"

#pragma endregion




#endif

