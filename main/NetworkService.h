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

#include <MQTTClient.h>
#include <MQTTLinux.h>

#define NET_DEBUG_PRINT(...)  ESP_LOGI("BUTTON NET", __VA_ARGS__);
#define NET_INFO_PRINT(...)   ESP_LOGI("BUTTON NET", __VA_ARGS__);

class NetworkService: public Task {
public:
	NetworkService(Service &svc);
	virtual ~NetworkService();

	bool begin();
	bool reconnect();
	void run();
	void runAsync(void *taskData);
	bool stop();

private:

	Service &svc_;

	Network mqttNetwork;
	MQTTClient mqttClient;
	MQTTPacket_connectData mqttConnectData;
	unsigned char mqttSendBuffer[256];
	unsigned char mqttReadBuffer[256];

	EventGroupHandle_t mqttEventGroup_ = NULL;
	xQueueHandle telemetryDataQueue_ = NULL;

	void doTask(void* taskData);

	void initializeSntp();
	void obtainTime();
	void getTime(char *timeStr);
};

#endif /* MAIN_NETWORKSERVICE_H_ */
