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
#include "JSON.h"

#include <esp_log.h>
#include "freertos/queue.h"
#include "MagicButtonAnimation.h"

#define SVC_DEBUG_PRINT(...)  ESP_LOGI("GALLON SVC", __VA_ARGS__);
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

	void subscribeForRequestData(xQueueHandle *queue_ptr);
	void notifyResponse(std::string &jsonString);

private:

	AppSettingStorage &appSetting_;

	xQueueHandle requestDataQueue_ = 0, responseDataQueue = 0;

	void handleTouch(uint8_t tNo, touchpad_event_t evt);
	void handleResponse(std::string &jsonString);

	//MagicButtonCometAnimation *cometAnimation_ = NULL;
};

#endif /* MAIN_SERVICE_H_ */
