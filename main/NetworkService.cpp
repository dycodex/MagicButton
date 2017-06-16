/*
 * NetworkService.cpp
 *
 *  Created on: Jun 12, 2017
 *      Author: andri
 */

#include "NetworkService.h"

#define MQTT_STOP_REQ_EVT 	BIT1

NetworkService::NetworkService(Service& svc):
Task(1, "netSvcTask", 4096*2, 2), svc_(svc) {
}

NetworkService::~NetworkService() {
	if (telemetryDataQueue_ != NULL) {
		vQueueDelete(telemetryDataQueue_);
	}
}

static void messageHandler_func(MessageData *md) {
	NET_DEBUG_PRINT("Subscription received!: %.*s", md->topicName->lenstring.len, md->topicName->lenstring.data);
}

bool NetworkService::begin() {

	if (mqttEventGroup_ == NULL) {
		mqttEventGroup_ = xEventGroupCreate();
	}

	xEventGroupClearBits(mqttEventGroup_, MQTT_STOP_REQ_EVT);

	NET_DEBUG_PRINT("Starting networking...");
	NetworkInit(&mqttNetwork);

	NET_DEBUG_PRINT("MQTTClientInit  ...");
	MQTTClientInit(&mqttClient, &mqttNetwork,
			10000,            		// command_timeout_ms
			mqttSendBuffer,         //sendbuf,
			sizeof(mqttSendBuffer), //sendbuf_size,
			mqttReadBuffer,         //readbuf,
			sizeof(mqttReadBuffer)  //readbuf_size
			);

	MQTTString clientId = MQTTString_initializer;
	clientId.cstring = svc_.getAppSetting().stuff.device.id;// "ANDRI-MQTT-CLIENT1";

	mqttConnectData = MQTTPacket_connectData_initializer;
	mqttConnectData.clientID = clientId;
	mqttConnectData.willFlag = 0;
	mqttConnectData.MQTTVersion = 3;
	mqttConnectData.keepAliveInterval = 10;
	mqttConnectData.cleansession = 1;

	return true;
}

bool NetworkService::reconnect() {

	int rc;

	NET_DEBUG_PRINT("NetworkConnect  ...");

	char* address = svc_.getAppSetting().stuff.config.mqttServer;//"iot.eclipse.org";
	if ((rc = NetworkConnect(&mqttNetwork, address, svc_.getAppSetting().stuff.config.mqttPort)) != 0) {
		NET_DEBUG_PRINT("Return code from network connect is %d\n", rc);
		return false;
	}

	NET_DEBUG_PRINT("MQTTConnect  ...");
	rc = MQTTConnect(&mqttClient, &mqttConnectData);
	if (rc != SUCCESS) {
		NET_DEBUG_PRINT("MQTTConnect: %d", rc);
	}

	std::string _prefTopic = std::string(svc_.getAppSetting().stuff.config.venueId) + "/" + std::string(svc_.getAppSetting().stuff.device.id);
	std::string _respTopic = _prefTopic + "/response";

	NET_DEBUG_PRINT("MQTTSubscribe to: %s", _respTopic.c_str());

	rc = MQTTSubscribe(&mqttClient, _respTopic.c_str(), QOS1, messageHandler_func);
	if (rc != SUCCESS) {
		NET_DEBUG_PRINT("MQTTSubscribe: %d", rc);
	}

	return true;
}

bool NetworkService::stop() {
	MQTTDisconnect(&mqttClient);
	NetworkDisconnect(&mqttNetwork);

	xEventGroupSetBits(mqttEventGroup_, MQTT_STOP_REQ_EVT);

	return true;
}

void NetworkService::run() {

	if (!mqttClient.isconnected) {
		reconnect();
		return;
	}

	if (this->telemetryDataQueue_ == 0) {
		return;
	}

	int rc;

	StuffAttr_t stuffAttr;
	if (xQueueReceive(this->telemetryDataQueue_, &stuffAttr, 2000/portTICK_PERIOD_MS) == pdFALSE) {
		//NET_DEBUG_PRINT("NO DATA QUEUED");
		return;
	}

	char strftime_buf[64];
	getTime(stuffAttr.datetime);

	MQTTMessage message;

	size_t msgLen = stuffAttr.toJson().length();
	char payload[msgLen];

	message.qos = QOS1;
	message.retained = 0;
	message.payload = payload;
	//sprintf(payload, "message number %d", count);
	strcpy(payload, stuffAttr.toJson().c_str());
	NET_DEBUG_PRINT("MQTT payload: %s", payload);

	message.payloadlen = msgLen;//strlen(payload);

	std::string _prefTopic = std::string(svc_.getAppSetting().stuff.config.venueId) + "/" + std::string(svc_.getAppSetting().stuff.device.id);
	std::string _pubpTopic = _prefTopic + "/buttons";
	NET_DEBUG_PRINT("MQTT Topic: %s", _pubpTopic.c_str());

	if ((rc = MQTTPublish(&mqttClient, _pubpTopic.c_str(), &message)) != 0) {
		NET_DEBUG_PRINT("Return code from MQTT publish is %d\n", rc);
	} else {
		NET_DEBUG_PRINT("MQTT published");
	}

//	MQTTYield(&mqttClient, 1000);
//	vTaskDelay(3000/portTICK_PERIOD_MS);
}

void NetworkService::runAsync(void* taskData) {
	doTask(taskData);
}

void NetworkService::doTask(void* taskData) {

	if (telemetryDataQueue_ == NULL) {
		telemetryDataQueue_ = xQueueCreate(10, sizeof(StuffAttr_t));
	}

	svc_.subscribeTelemetryData(&telemetryDataQueue_);

	//deal with time
	char strftime_buf[64];
	getTime(strftime_buf);
	NET_DEBUG_PRINT("The current date/time: %s", strftime_buf);

	reconnect();

	while(1) {
		run();

		EventBits_t uxBits;
		uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_STOP_REQ_EVT, false, false, 0);
		if (uxBits & MQTT_STOP_REQ_EVT) {
			NET_DEBUG_PRINT("MQTT stop.");
			// Clear stop event bit
			xEventGroupClearBits(mqttEventGroup_, MQTT_STOP_REQ_EVT);
			break;
		}

		vTaskDelay(10/portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}

void NetworkService::initializeSntp() {
	NET_DEBUG_PRINT("Initializing SNTP...");
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();
}

void NetworkService::obtainTime() {
	initializeSntp();

	// wait for time to be set
	time_t now = 0;
	struct tm timeinfo = { 0 };
	int retry = 0;
	const int retry_count = 10;
	while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
		NET_DEBUG_PRINT("Waiting for system time to be set... (%d/%d)", retry, retry_count);
		vTaskDelay(2000 / portTICK_PERIOD_MS);
		time(&now);
		localtime_r(&now, &timeinfo);
	}
}

void NetworkService::getTime(char *timeStr) {
	//deal with time
	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);
	// Is time set? If not, tm_year will be (1970 - 1900).
	if (timeinfo.tm_year < (2016 - 1900)) {
		NET_DEBUG_PRINT("Time is not set yet. Connecting to WiFi and getting time over NTP.");
		obtainTime();
		// update 'now' variable with current time
		time(&now);
	}

	char strftime_buf[64];
	// Set timezone to Eastern Standard Time and print local time
	//setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
	//setenv("TZ", "Etc/GMT-7", 1);
	setenv("TZ", "UTC", 1);
	tzset();
	localtime_r(&now, &timeinfo);
	//strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%dT%H:%M:%S.000Z", &timeinfo);
	//NET_DEBUG_PRINT("The current date/time: %s", strftime_buf);

	strcpy(timeStr, strftime_buf);
}
