/*
 * AppSetting.cpp
 *
 *  Created on: Jun 13, 2017
 *      Author: andri
 */

#include "AppSetting.h"

AppSettingStorage::AppSettingStorage() {

}

AppSettingStorage::~AppSettingStorage() {
}

esp_err_t AppSettingStorage::begin() {
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
		// NVS partition was truncated and needs to be erased
		const esp_partition_t* nvs_partition = esp_partition_find_first(
				ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
		assert(nvs_partition && "partition table must have an NVS partition");
		ESP_ERROR_CHECK( esp_partition_erase_range(nvs_partition, 0, nvs_partition->size) );
		// Retry nvs_flash_init
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK( err );

	// Open
	err = nvs_open(APPSETTING_NAMESPACE, NVS_READWRITE, &nvsHandle_);
	return err;
}

esp_err_t AppSettingStorage::load() {
	if (nvsHandle_ == NULL) {
		APPSETTING_DEBUG_PRINT("AppSetting not init-ed");
		return ESP_FAIL;
	}

	size_t size = sizeof(Stuff_t);
	esp_err_t err = nvs_get_blob(nvsHandle_, APPSETTING_KEY, &stuff, &size);

	if (size == 0) {
		APPSETTING_DEBUG_PRINT("Nothing saved yet!\n");
	} else {
		APPSETTING_DEBUG_PRINT("Stored %d", size);
	}

	return err;
}
esp_err_t AppSettingStorage::initSetting() {
	if (nvsHandle_ == NULL) {
		APPSETTING_DEBUG_PRINT("AppSetting not init-ed");
		return ESP_FAIL;
	}

	stuff.init();
	return save();
}

esp_err_t AppSettingStorage::save() {
	if (nvsHandle_ == NULL) {
		APPSETTING_DEBUG_PRINT("AppSetting not init-ed");
		return ESP_FAIL;
	}

	size_t size = sizeof(Stuff_t);
	esp_err_t err = nvs_set_blob(nvsHandle_, APPSETTING_KEY, &stuff, size);

	return err;
}

void AppSettingStorage::printVals() {
	APPSETTING_DEBUG_PRINT("configured: %d", stuff.config.configured);
	APPSETTING_DEBUG_PRINT("wifiConfigured: %d", stuff.config.wifiConfigured);
	APPSETTING_DEBUG_PRINT("configInitialized: %d", stuff.config.configInitialized);
	APPSETTING_DEBUG_PRINT("apName: %s", stuff.config.apName);
	APPSETTING_DEBUG_PRINT("apPass: %s", stuff.config.apPass);
}

AppSettingStorage AppSetting;

