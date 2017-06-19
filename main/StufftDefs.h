//
// Created by Andri Yadi on 12/4/15.
//

#ifndef GALLON_STUFFDEFS_H
#define GALLON_STUFFDEFS_H

#include <Arduino.h>
#include "string.h"
#include "Constants.h"

struct StuffAttr_t {
	uint8_t buttonId;
	char requestId[16];
	char datetime[30];

	String toJson() {
		//create json
		String json = "{";
		json += "\"requestid\": \"" + String(requestId) + "\",";
		json += "\"datetime\": \"" + String(datetime) + "\",";
		json += "\"buttonid\": " + String(buttonId);
		json += "}";

		return json;
	}
};

struct StuffConfig_t {
	
	char ssidName[30];        //sn
	char ssidPass[30];        //sp
	char ipAddress[15];
	boolean configured = 0;
	boolean wifiConfigured = 0;
	boolean configInitialized = 0;
	char apName[20];
	char apPass[10];
	char mqttServer[100];
	int mqttPort;
	boolean appPaired = 0;
	char venueId[30];

	String toJson() {
		//create json

		String json = "{";
		json += (wifiConfigured) ?
				"\"ssidName\": \"" + String(ssidName) + "\"," : "";
		json += "\"ipAddress\": \"" + String(ipAddress) + "\",";
		json += "\"wifiConfigured\": " + String(wifiConfigured) + "\",";
		json += "\"venueId\": " + String(venueId);
		json += "}";

		return json;
	}
};

struct StuffDevice_t {
	
	char id[40];
	char name[30];
	char types[30];
	char subtypes[30];
	int buttonCount = BUTTON_COUNT;
	//byte sensorProbesCount;

	String toJson() {
		//create json
		String json = "{";
		json += "\"id\": \"" + String(id) + "\",";
		json += "\"name\": \"" + String(name) + "\",";
		json += "\"types\": \"" + String(types) + "\",";
		//json += "\"sensorProbesCount\": " + String(sensorProbesCount) + ",";
		json += "\"buttoncount\":" + String(buttonCount, 10) + ",";
		json += "\"subtypes\": \"" + String(subtypes) + "\"";
		json += "}";

		return json;
	}
};

struct StuffAccount {
	char userId[30];
	char userToken[32];
	uint32_t tokenExpiry;
};

struct Stuff_t {

	StuffDevice_t device;

	StuffConfig_t config;
	StuffAttr_t attr;

	StuffAccount account;
	uint16_t crc;

	void init() {

//		attr = {
//				100,
//				20,
//				0
//		};

//		device = {
//				"x61110a6-4d0a-4382-bb73-534204fef25z",
//				"My Smart Gallon",
//				"SmartGallon",
//				"Generic"
//		};

		strcpy(device.id, DEFAULT_DEVICE_ID);
		strcpy(device.name, DEFAULT_DEVICE_NAME);
		strcpy(device.types, DEFAULT_DEVICE_TYPES);
		strcpy(device.subtypes, DEFAULT_DEVICE_SUBTYPES);

		strcpy(config.venueId, DEFAULT_VENUE_ID);
		strcpy(config.apName, DEFAULT_AP_NAME);
		strcpy(config.apPass, DEFAULT_AP_PASS);
		strcpy(config.ssidName, DEFAULT_SSID_NAME);
		strcpy(config.ssidPass, DEFAULT_SSID_PASS);
		strcpy(config.ipAddress, DEFAULT_IP_ADDR);

		config.configured = false;
		config.configInitialized = true;
		config.mqttPort = MQTT_PORT;

		strncpy(account.userId, "dycode", 30);
		strncpy(account.userToken, "DaegCen", 32);

		crc = ~0;

#if TEST_MODE
		strcpy(config.mqttServer, MQTT_SERVER);
		config.wifiConfigured = true;

		strcpy(account.userId, "dycodeiot.azure-devices.net/gallon-01");
		strcpy(account.userToken, "SharedAccessSignature sr=dycodeiot.azure-devices.net/devices/gallon-01&sig=lG1RlLpC8uyq6zoPtKI4FfCoFCR7QyZfaPuzhEEFVxM%3D&se=1459525685");
		account.tokenExpiry = 1459525685;

#else
		strcpy(config.mqttServer, MQTT_SERVER);
		config.wifiConfigured = false;

//		strcpy(account.userId, "");
//		strcpy(account.userToken, "");
//		account.tokenExpiry = 0;
#endif
	}

	String toJson() {
		//create json
		String json = "{";
		json += "\"device\": " + String(device.toJson()) + ",";
		json += "\"attr\": " + String(attr.toJson()) + ",";
		json += "\"config\": " + String(config.toJson());
		json += "}";

		return json;
	}
};

#endif //GALLON_STUFFDEFS_H
