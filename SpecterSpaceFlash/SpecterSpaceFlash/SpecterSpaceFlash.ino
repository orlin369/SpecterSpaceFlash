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

#pragma region Headres

#include "GeneralHelper.h"
#include "DeviceConfiguration.h"
#include "DebugPort.h"
#include "ApplicationConfiguration.h"
#include "LocalWebServer.h"

// Builtin functionalities.
#include <ESP8266WiFi.h>
#include <FS.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#ifdef ENABLE_SELF_OTA

#include <ESP8266httpUpdate.h>

#endif // ENABLE_SELF_OTA

#ifdef ENABLE_OTA_ARDUINO

#include <ArduinoOTA.h>

#endif // ENABLE_OTA_ARDUINO

#pragma endregion

#pragma region Variables

/* @brief WiFi mode. */
WiFiMode_t WiFiMode_g = WiFiMode_t::WIFI_OFF;

/* @brief WiFi disconnected since. */
long WiFiDisconnectedSince_g = 0;

/* @brief On station mode connected handler. */
WiFiEventHandler OnStationModeConnectedHandler_g;

/* @brief On station mode disconnected handler. */
WiFiEventHandler OnStationModeDisconnectedHandler_g;

/* @brief On station mode got IP handler. */
WiFiEventHandler OnStationModeGotIPHandler_g;

/* @brief On AP mode station connected. */
WiFiEventHandler OnAPModeStationConnectedHandle_g;

/* @brief On AP mode station disconnected. */
WiFiEventHandler OnAPModeStationDisconnectedHandle_g;

#ifdef ENABLE_CAYENNE_MODE

/* @brief On AP mode station disconnected current time. */
unsigned long OnlineTime_g;

/* @brief On AP mode station disconnected previous time. */
unsigned long PreviousTime_g;

/* @brief Is connected to internet flag. */
bool IsConnectedToInternet_g = false;

#endif

#pragma endregion


#pragma region Prototypes

/** @brief Printout in the debug console device status.
 *  @return Void.
 */
void device_properties();

/** @brief Configure the file system.
 *  @return Void.
 */
void configure_file_system();

/** @brief Configure WiFi module to access point.
 *  @return Void.
 */
void configure_to_ap();

/** @brief Handler that execute when client is connected.
 *  @param const WiFiEventSoftAPModeStationConnected& evt, Callback handler
 *  @return Void.
 */
void handler_ap_mode_station_connected(const WiFiEventSoftAPModeStationConnected& evt);

/** @brief Handler that execute when client is disconnected.
 *  @param const WiFiEventSoftAPModeStationDisconnected& evt, Callback handler
 *  @return Void.
 */
void handler_ap_mode_station_disconnected(const WiFiEventSoftAPModeStationDisconnected& evt);

/** @brief Configure WiFi module to station.
 *  @return Void.
 */
void configure_to_sta();

/** @brief Handler that execute when the device is connected.
 *  @param evt WiFiEventStationModeConnected, Data object.
 *  @return Void.
 */
void handler_sta_mode_connected(WiFiEventStationModeConnected evt);

/** @brief Handler that execute when the device got IP.
 *  @param evt WiFiEventStationModeGotIP, Callback handler.
 *  @return Void.
 */
void handler_sta_mode_got_ip(WiFiEventStationModeGotIP evt);

/** @brief Handler that execute when the device is disconnected.
 *  @param evt WiFiEventStationModeDisconnected, Callback handler.
 *  @return Void.
 */
void handler_sta_mode_disconnected(WiFiEventStationModeDisconnected evt);

/** @brief Software shutdown procedure.
 *  @return Void.
 */
void shutdown();

#ifdef ENABLE_SELF_OTA

/** @brief Check for updates.
 *  @return Void.
 */
void check_update();

#endif // ENABLE_SELF_OTA

#pragma endregion

/** @brief Setup function.
 *  @return Void.
 */
void setup()
{
	configure_debug_port();

	device_properties();

	// Turn off the WiFi module.
	WiFi.mode(WiFiMode_g);

	// Start the file system.
	configure_file_system();
}

void loop()
{

}


#pragma region Functions

/** @brief Printout in the debug console flash state.
 *  @return Void.
 */
void device_properties()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	DEBUGLOG("Flash chip size: %u\r\n", ESP.getFlashChipRealSize());
	DEBUGLOG("Sketch size: %u\r\n", ESP.getSketchSize());
	DEBUGLOG("Free flash space: %u\r\n", ESP.getFreeSketchSpace());
	DEBUGLOG("Free heap: %d\r\n", ESP.getFreeHeap());
	DEBUGLOG("Firmware version: %d\r\n", FW_VERSION);
	DEBUGLOG("SDK version: %s\r\n", ESP.getSdkVersion());
	uint8 c_MACL[WL_MAC_ADDR_LENGTH];
	WiFi.macAddress(c_MACL);
	String MACL = mac2str(c_MACL);
	DEBUGLOG("MAC address: %s\r\n", MACL.c_str());
}

/** @brief Configure the file system.
 *  @param fileSystem FS, File system object.
 *  @return Void.
 */
void configure_file_system()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	if (!SPIFFS.begin())
	{
		DEBUGLOG("Can not load file system.\r\n");
		for (;;) {}
	}

#ifdef EANBLE_DEBUG_OUT

	// List files
	Dir dir = SPIFFS.openDir("/");
	while (dir.next()) {
		String fileName = dir.fileName();
		size_t fileSize = dir.fileSize();
		DEBUGLOG("FS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
	}
	DEBUGLOG("\r\n");

#endif // EANBLE_DEBUG_OUT
}

#pragma region AP mode

/** @brief Configure WiFi module to access point.
 *  @return Void.
 */
void configure_to_ap()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	// Register event handlers.
	// Callback functions will be called as long as these handler objects exist.
	// Call "handler_ap_mode_station_connected" each time a station connects
	OnAPModeStationConnectedHandle_g = WiFi.onSoftAPModeStationConnected(&handler_ap_mode_station_connected);

	// Call "handler_ap_mode_station_disconnected" each time a station disconnects
	OnAPModeStationDisconnectedHandle_g = WiFi.onSoftAPModeStationDisconnected(&handler_ap_mode_station_disconnected);

	// Set the host name.
	WiFi.hostname(DeviceConfiguration.DeviceName.c_str());

	// Set the mode.
	WiFi.mode(WIFI_AP);

	// NOTE: See description, this can be used as unique identification of the device.
	String SSIDL = DeviceConfiguration.DeviceName + String(" ") + (String)ESP.getChipId();

	// Create AP name.
	DEBUGLOG("SSIDL: %s\r\n", SSIDL.c_str());

	WiFi.softAP(SSIDL.c_str(), DeviceConfiguration.APPassword.c_str());
	DEBUGLOG("APPassword: %s\r\n", DeviceConfiguration.APPassword.c_str());
}

/** @brief Handler that execute when client is connected.
 *  @param const WiFiEventSoftAPModeStationConnected& evt, Callback handler
 *  @return Void.
 */
void handler_ap_mode_station_connected(const WiFiEventSoftAPModeStationConnected& evt)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	DEBUGLOG("Station connected: %s\r\n", mac2str(evt.mac).c_str());
}

/** @brief Handler that execute when client is disconnected.
 *  @param const WiFiEventSoftAPModeStationDisconnected& evt, Callback handler
 *  @return Void.
 */
void handler_ap_mode_station_disconnected(const WiFiEventSoftAPModeStationDisconnected& evt)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	DEBUGLOG("Station disconnected: %s\r\n", mac2str(evt.mac).c_str());
}

#pragma endregion

#pragma region STA mode

/** @brief Configure WiFi module to station.
 *  @return Void.
 */
void configure_to_sta()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	// Register WiFi Event to control connection LED.
	OnStationModeConnectedHandler_g = WiFi.onStationModeConnected(
		[](WiFiEventStationModeConnected evt) {
		handler_sta_mode_connected(evt);
	});

	OnStationModeDisconnectedHandler_g = WiFi.onStationModeDisconnected(
		[](WiFiEventStationModeDisconnected evt) {
		handler_sta_mode_disconnected(evt);
	});

	OnStationModeGotIPHandler_g = WiFi.onStationModeGotIP(
		[](WiFiEventStationModeGotIP evt) {
		handler_sta_mode_got_ip(evt);
	});

	// Set the host name.
	WiFi.hostname(DeviceConfiguration.DeviceName.c_str());

	//disconnect required here
	//improves reconnect reliability
	WiFi.disconnect();

	// Encourage clean recovery after disconnect species5618, 08-March-2018
	WiFi.setAutoReconnect(true);
	WiFi.mode(WIFI_STA);

	DEBUGLOG("WiFi Connecting: %s\r\n", DeviceConfiguration.STASSID.c_str());

	WiFi.begin(DeviceConfiguration.STASSID.c_str(), DeviceConfiguration.STAPassword.c_str());

	// Check the DHCP.
	//if (!DeviceConfiguration.dhcp) {
	//	DEBUGLOG("NO DHCP\r\n");
	//	WiFi.config(DeviceConfiguration.ip, DeviceConfiguration.gateway, DeviceConfiguration.netmask, DeviceConfiguration.dns);
	//}

	WiFi.waitForConnectResult();
}

/** @brief Handler that execute when the device is connected.
 *  @param evt WiFiEventStationModeConnected, Data object.
 *  @return Void.
 */
void handler_sta_mode_connected(WiFiEventStationModeConnected evt)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	DEBUGLOG("WiFi Connected: %s\r\n", DeviceConfiguration.STASSID.c_str());
	DEBUGLOG("Waiting for DHCP\r\n");

	WiFiDisconnectedSince_g = 0;
}

/** @brief Handler that execute when the device got IP.
 *  @param evt WiFiEventStationModeGotIP, Callback handler.
 *  @return Void.
 */
void handler_sta_mode_got_ip(WiFiEventStationModeGotIP evt)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	DEBUGLOG("IP Address:      %s\r\n", WiFi.localIP().toString().c_str());
	DEBUGLOG("Gateway:         %s\r\n", WiFi.gatewayIP().toString().c_str());
	DEBUGLOG("DNS:             %s\r\n", WiFi.dnsIP().toString().c_str());

	WiFiDisconnectedSince_g = 0;
}

/** @brief Handler that execute when the device is disconnected.
 *  @param evt WiFiEventStationModeDisconnected, Callback handler.
 *  @return Void.
 */
void handler_sta_mode_disconnected(WiFiEventStationModeDisconnected evt)
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	if (WiFiDisconnectedSince_g == 0) {
		WiFiDisconnectedSince_g = millis();
	}

	int DisconnectedTimeL = (int)((millis() - WiFiDisconnectedSince_g) / 1000);

	DEBUGLOG("Disconnected for %d seconds.\r\n", DisconnectedTimeL);
}

#pragma endregion

#pragma region OTA Updates

#ifdef ENABLE_SELF_OTA

/** @brief Check for updates.
 *  @return Void.
 */
void check_update()
{
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	uint8 c_MACL[WL_MAC_ADDR_LENGTH];
	WiFi.macAddress(c_MACL);
	String MACL = mac2str(c_MACL);

	String BinariImageUrlL = String(SERVER_DOMAIN) + String(UPDATE_SERVER_PATH);
	String FwVersUrlL = String(SERVER_DOMAIN) + String(VERSION_SERVER_PATH);

	DEBUGLOG("Checking for firmware updates.\r\n");
	DEBUGLOG("MAC address: %s\r\n", MACL.c_str());
	DEBUGLOG("Firmware version URL: %s\r\n", FwVersUrlL.c_str());

	HTTPClient HttpClientL;

	int StatusCodeL;

	HttpClientL.begin(FwVersUrlL);
	StatusCodeL = HttpClientL.GET();

	if (StatusCodeL == 200)
	{
		String NewFwVersionL = HttpClientL.getString();
		int VersionNumberL = NewFwVersionL.toInt();

		DEBUGLOG("Current   firmware version: %d\r\n", FW_VERSION);
		DEBUGLOG("Available firmware version: %d\r\n", VersionNumberL);

		if (VersionNumberL != FW_VERSION)
		{
			DEBUGLOG("Preparing to update\r\n");
			DEBUGLOG("Binary file URL : %s\r\n", BinariImageUrlL.c_str());

			t_httpUpdate_return ResponseL = ESPhttpUpdate.update(BinariImageUrlL, String(FW_VERSION));

			switch (ResponseL)
			{
			case HTTP_UPDATE_FAILED:
				DEBUGLOG("HTTP_UPDATE_FAILD Error (%d): %s\r\n",
					ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
				break;

			case HTTP_UPDATE_NO_UPDATES:
				DEBUGLOG("HTTP_UPDATE_NO_UPDATES\r\n");
				break;

			case HTTP_UPDATE_OK:
				DEBUGLOG("HTTP_UPDATE_OK\r\n");
				break;
			}
		}
		else
		{
			DEBUGLOG("Already on latest version.\r\n");
		}
	}
	else
	{
		DEBUGLOG("Firmware version check failed, got HTTP response code %d\r\n", StatusCodeL);
	}

	HttpClientL.end();
}

#endif // ENABLE_SELF_OTA

#pragma endregion

#pragma endregion

