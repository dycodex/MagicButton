/*
 * NetworkService.h
 *
 *  Created on: Jun 12, 2017
 *      Author: andri
 */

#ifndef MAIN_NETWORKSERVICE_H_
#define MAIN_NETWORKSERVICE_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include <string.h>

#include <time.h>
#include <sys/time.h>
#include "apps/sntp/sntp.h"

#include <functional>

#include "Task.h"
#include "Service.h"

#include "sdkconfig.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

extern "C" {
	#include "mqtt.h"
}

#define NET_DEBUG_PRINT(...)  ESP_LOGI("BUTTON NET", __VA_ARGS__);
#define NET_INFO_PRINT(...)   ESP_LOGI("BUTTON NET", __VA_ARGS__);

typedef struct mqtt_subscription_data_t {
		mqtt_client *client;
		std::string topic;
		std::string payload;
};

class NetworkService: public Task {
public:

	typedef std::function<void()> EventCallback;
	typedef std::function<void(mqtt_subscription_data_t &subsData)> SubscriptionDataAvailableCallback;

	NetworkService(Service &svc);
	virtual ~NetworkService();

	bool begin();
	bool reconnect();
	void run();
	void runAsync(void *taskData);

	void start(void *taskData=nullptr);
	bool stop();

	void onSubscriptionDataAvailable(SubscriptionDataAvailableCallback cb) {
		subscriptionDataAvailableCallback_ = cb;
	}

	void onReady(EventCallback cb) {
		networkServiceReadyCallback_ = cb;
	}

private:

	Service &svc_;
	mqtt_settings mqttSettings_ = {};
	mqtt_client *mqttClient_ = nullptr;

	EventCallback networkServiceReadyCallback_ = NULL;

	unsigned char mqttSendBuffer[256];
	unsigned char mqttReadBuffer[256];

	//EventGroupHandle_t mqttEventGroup_ = NULL;
	xQueueHandle requestDataQueue_ = NULL;

	void doTask(void* taskData);
	void registerToCloud();
	void initializeSntp();
	void obtainTime();
	void getTime(char *timeStr);

	//mqtt_subscription_data_t lastSubcriptionData_;
	SubscriptionDataAvailableCallback subscriptionDataAvailableCallback_ = NULL;
	void handleSubscriptionData(mqtt_subscription_data_t &subdata);
};

#endif /* MAIN_NETWORKSERVICE_H_ */
