/*
 * Service.h
 *
 *  Created on: Jun 10, 2017
 *      Author: andri
 */

#ifndef MAIN_SERVICE_H_
#define MAIN_SERVICE_H_

#include "Constants.h"
#include "StufftDefs.h"
#include "AppSetting.h"
#include "MagicButton.h"
#include "Task.h"

#include <esp_log.h>
#include "freertos/queue.h"

#define SVC_DEBUG_PRINT(...)  ESP_LOGD("GALLON SVC", __VA_ARGS__);
#define SVC_INFO_PRINT(...)   ESP_LOGI("GALLON SVC", __VA_ARGS__);

class Service: public Task {
public:
	Service(AppSettingStorage &appSetting);
	virtual ~Service();

	void begin();

	void startServing();
	void stopServing();
	void run();
	void runAsync(void *taskData);

	AppSettingStorage &getAppSetting() {
		return appSetting_;
	}

	void subscribeTelemetryData(xQueueHandle *queue_ptr);

private:

	AppSettingStorage &appSetting_;

	xQueueHandle telemetryDataQueue_ = NULL;

	void handleTouch(uint8_t tNo, touchpad_event_t evt);
};

#endif /* MAIN_SERVICE_H_ */
