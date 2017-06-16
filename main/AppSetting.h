/*
 * AppSetting.h
 *
 *  Created on: Jun 13, 2017
 *      Author: andri
 */

#ifndef MAIN_APPSETTING_H_
#define MAIN_APPSETTING_H_

#include "StufftDefs.h"
#include "esp_system.h"
#include "esp_partition.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

#define APPSETTING_DEBUG_PRINT(...)  ESP_LOGD("APPSETTING", __VA_ARGS__);
#define APPSETTING_INFO_PRINT(...)   ESP_LOGI("APPSETTING", __VA_ARGS__);

#define APPSETTING_NAMESPACE	"AppSetting"
#define APPSETTING_KEY			"STUFF"

class AppSettingStorage {
public:
	AppSettingStorage();
	virtual ~AppSettingStorage();

	esp_err_t begin();
	esp_err_t initSetting();
	esp_err_t load();
	esp_err_t save();
	void printVals();

	Stuff_t stuff;

private:
	nvs_handle nvsHandle_ = NULL;
};

extern AppSettingStorage AppSetting;

#endif /* MAIN_APPSETTING_H_ */
