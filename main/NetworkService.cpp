/*
 * NetworkService.cpp
 *
 *  Created on: Jun 12, 2017
 *      Author: andri
 */

#include "NetworkService.h"

#define MQTT_CONNECTED_EVT 	BIT0
#define MQTT_STOP_REQ_EVT 	BIT1
#define MQTT_SUBSDATA_EVT 	BIT2

//static subscription_data_t subscriptionData;
//static xQueueHandle subscriptionDataQueue_ = NULL;

static EventGroupHandle_t mqttEventGroup_ = NULL;
static mqtt_subscription_data_t lastSubcriptionData_;

static void connected_cb(void *self, void *params)
{
	NET_DEBUG_PRINT("MQTT Connected\n");

//    mqtt_client *client = (mqtt_client *)self;
//    mqtt_subscribe(client, "/test-andri-sub", 0);
//    mqtt_publish(client, "/test-andri", "howdy!", 6, 0, 0);

	xEventGroupSetBits(mqttEventGroup_, MQTT_CONNECTED_EVT);
}
static void disconnected_cb(void *self, void *params)
{
	NET_DEBUG_PRINT("MQTT Disconnected\n");
	xEventGroupClearBits(mqttEventGroup_, MQTT_CONNECTED_EVT);
}
static void reconnect_cb(void *self, void *params)
{
	xEventGroupClearBits(mqttEventGroup_, MQTT_CONNECTED_EVT);
}
static void subscribe_cb(void *self, void *params)
{
	NET_DEBUG_PRINT("MQTT Subscribe OK\n");
}
static void publish_cb(void *self, void *params)
{

}
static void data_cb(void *self, void *params)
{
//	NET_DEBUG_PRINT("Got subs data");

    mqtt_client *client = (mqtt_client *)self;
    mqtt_event_data_t *event_data = (mqtt_event_data_t *)params;

    /*
    if (subscriptionDataQueue_) {

    	mqtt_subscription_data_t subscriptionData = {};
    	subscriptionData.client = client;

        if (event_data->data_offset == 0) {

            char *topic = (char *)malloc(event_data->topic_length + 1);
            memcpy(topic, event_data->topic, event_data->topic_length);
            topic[event_data->topic_length] = 0;
//            NET_DEBUG_PRINT("Sub topic: %s\n", topic);
            subscriptionData.topic.assign(topic, event_data->topic_length + 1);
            free(topic);
        }

        char *data = malloc(event_data->data_length + 1);
        memcpy(data, event_data->data, event_data->data_length);
        data[event_data->data_length] = 0;
//        subscriptionData.payload = std::string(data);
        subscriptionData.payload.assign(data, event_data->data_length + 1);
        free(data);

        xQueueSend(subscriptionDataQueue_, &subscriptionData, 0);
    }
    */

    //save to lastSubcriptionData_

    lastSubcriptionData_.client = client;

	if (event_data->data_offset == 0) {

		char *topic = (char *)malloc(event_data->topic_length + 1);
		memcpy(topic, event_data->topic, event_data->topic_length);
		topic[event_data->topic_length] = 0;
//      NET_DEBUG_PRINT("Sub topic: %s\n", topic);
		lastSubcriptionData_.topic.assign(topic, event_data->topic_length + 1);
		free(topic);
	}

	char *data = malloc(event_data->data_length + 1);
	memcpy(data, event_data->data, event_data->data_length);
	data[event_data->data_length] = 0;
//  subscriptionData.payload = std::string(data);
	lastSubcriptionData_.payload.assign(data, event_data->data_length + 1);
	free(data);

    xEventGroupSetBits(mqttEventGroup_, MQTT_SUBSDATA_EVT);
}

NetworkService::NetworkService(Service& svc):
Task(1, "netSvcTask", 4096*2, 2), svc_(svc) {
}

NetworkService::~NetworkService() {
	stop();
//	if (requestDataQueue_ != NULL) {
//		vQueueDelete(requestDataQueue_);
//	}
}

bool NetworkService::begin() {

	if (mqttEventGroup_ == NULL) {
		mqttEventGroup_ = xEventGroupCreate();
	}

	xEventGroupClearBits(mqttEventGroup_, MQTT_STOP_REQ_EVT);
	xEventGroupClearBits(mqttEventGroup_, MQTT_SUBSDATA_EVT);

//	if (subscriptionDataQueue_ == 0) {
//		subscriptionDataQueue_ = xQueueCreate(10, sizeof(mqtt_subscription_data_t));
//	}

	NET_DEBUG_PRINT("Starting MQTT...");

	mqttSettings_.connected_cb = connected_cb;
	mqttSettings_.disconnected_cb = disconnected_cb;
	mqttSettings_.reconnect_cb = reconnect_cb,
	mqttSettings_.subscribe_cb = subscribe_cb,
	mqttSettings_.publish_cb = publish_cb,
	mqttSettings_.data_cb = data_cb,
	mqttSettings_.port = svc_.getAppSetting().stuff.config.mqttPort;
	char* address = svc_.getAppSetting().stuff.config.mqttServer;
	strcpy(mqttSettings_.host, address);
	strcpy(mqttSettings_.client_id, svc_.getAppSetting().stuff.device.id);
	//strcpy(settings.username, svc_.getAppSetting().stuff.account.userId);
	//strcpy(settings.password, svc_.getAppSetting().stuff.account.userToken);
	strcpy(mqttSettings_.lwt_topic, "/lwt");
	strcpy(mqttSettings_.lwt_msg, "offline");
	mqttSettings_.lwt_qos = 0;
	mqttSettings_.lwt_retain = 0;

	return true;
}

bool NetworkService::reconnect() {

	//wait for connection

	EventBits_t uxBits;
	uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_CONNECTED_EVT, false, false, portMAX_DELAY);

	if (uxBits & MQTT_CONNECTED_EVT) {
		//NET_DEBUG_PRINT("MQTT Ready!");
	}

	std::string _prefTopic = std::string(svc_.getAppSetting().stuff.config.venueId) + "/" + std::string(svc_.getAppSetting().stuff.device.id);
	std::string _respTopic = _prefTopic + "/response";

	NET_DEBUG_PRINT("MQTT Subscribe to: %s", _respTopic.c_str());

	mqtt_subscribe(mqttClient_, _respTopic.c_str(), 0);

	registerToCloud();

	return true;
}

void NetworkService::start(void* taskData) {

	mqttClient_ = mqtt_start(&mqttSettings_);

	Task::start(taskData);
}

bool NetworkService::stop() {

	mqtt_stop();
	mqtt_destroy(mqttClient_);

	xEventGroupSetBits(mqttEventGroup_, MQTT_STOP_REQ_EVT);

	return true;
}

void NetworkService::run() {

//	if (!mqttClient.isconnected) {
//		reconnect();
//		return;
//	}

//	if (subscriptionDataQueue_ != 0) {
////		mqtt_subscription_data_t subsData;
//		if (xQueueReceive(subscriptionDataQueue_, &lastSubcriptionData_, 0)) {
//
//			//do something
//			handleSubscribedData(lastSubcriptionData_);
//			NET_DEBUG_PRINT("Queued new subscribed data");
//		}
//	}

	EventBits_t uxBits;

	uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_SUBSDATA_EVT, true, false, 10); //clear on exit
	if (uxBits & MQTT_SUBSDATA_EVT) {
		//NET_DEBUG_PRINT("MQTT Data Ready: %s", lastSubcriptionData_.topic.c_str());
		handleSubscriptionData(lastSubcriptionData_);
	}

	if (this->requestDataQueue_ == 0) {
		return;
	}

	//int rc;

	StuffAttr_t stuffAttr;
	if (xQueueReceive(this->requestDataQueue_, &stuffAttr, 2000/portTICK_PERIOD_MS) == pdFALSE) {
		//NET_DEBUG_PRINT("NO DATA QUEUED");
		return;
	}

	//EventBits_t uxBits;
	uxBits = xEventGroupWaitBits(mqttEventGroup_, MQTT_CONNECTED_EVT, false, false, portMAX_DELAY);

	if (uxBits & MQTT_CONNECTED_EVT) {
		//NET_DEBUG_PRINT("MQTT Ready!");
	}

	char strftime_buf[64];
	getTime(stuffAttr.datetime);

	size_t msgLen = stuffAttr.toJson().length();
	char payload[msgLen];

	//sprintf(payload, "message number %d", count);
	strcpy(payload, stuffAttr.toJson().c_str());
	NET_DEBUG_PRINT("MQTT payload: %s", payload);

	std::string _prefTopic = std::string(svc_.getAppSetting().stuff.config.venueId) + "/" + std::string(svc_.getAppSetting().stuff.device.id);
	std::string _pubpTopic = _prefTopic + "/buttons";
	NET_DEBUG_PRINT("MQTT Topic: %s", _pubpTopic.c_str());

	mqtt_publish(mqttClient_, _pubpTopic.c_str(), payload, msgLen, 0, 0);
}

void NetworkService::runAsync(void* taskData) {
	doTask(taskData);
}

void NetworkService::doTask(void* taskData) {

//	if (requestDataQueue_ == NULL) {
//		requestDataQueue_ = xQueueCreate(10, sizeof(StuffAttr_t));
//	}

	//Subscribe to service and get the data queue
	svc_.subscribeForRequestData(&requestDataQueue_);

	//deal with time
	char strftime_buf[64];
	getTime(strftime_buf);
	NET_DEBUG_PRINT("The current date/time: %s", strftime_buf);

	reconnect();

	//at this point, all is ready
	if (networkServiceReadyCallback_) {
		networkServiceReadyCallback_();
	}

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

void NetworkService::registerToCloud() {
	StuffDevice_t deviceData = svc_.getAppSetting().stuff.device;

	size_t length = deviceData.toJson().length();
	char payload[length];

	strcpy(payload, deviceData.toJson().c_str());

	NET_DEBUG_PRINT("MQTT payload: %s", payload);
	std::string _prefTopic = std::string(svc_.getAppSetting().stuff.config.venueId) + "/" + std::string(svc_.getAppSetting().stuff.device.id);
	std::string _pubpTopic = _prefTopic + "/reg";
	NET_DEBUG_PRINT("MQTT Topic: %s", _pubpTopic.c_str());

	mqtt_publish(mqttClient_, _pubpTopic.c_str(), payload, length, 0, 0);
}

void NetworkService::handleSubscriptionData(mqtt_subscription_data_t &subsData) {

	if (subsData.client != mqttClient_) {
		NET_DEBUG_PRINT("NOT for me");
		return;
	}

//	NET_DEBUG_PRINT("Sub topic: %s", subsData.topic.c_str());
//	NET_DEBUG_PRINT("Sub payload: %s", subsData.payload.c_str());

	if (subscriptionDataAvailableCallback_) {
		subscriptionDataAvailableCallback_(subsData);
	}

//	//if (subsData.topic.find_last_of("response") != std::string::npos) {
//		svc_.notifyResponse(subsData.payload);
//	//}
}
