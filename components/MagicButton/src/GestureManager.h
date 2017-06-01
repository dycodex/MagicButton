/*
 * GestureManager.h
 *
 *  Created on: May 12, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_MAGICBUTTON_SRC_GESTUREMANAGER_H_
#define COMPONENTS_MAGICBUTTON_SRC_GESTUREMANAGER_H_

#include <Arduino.h>
#include <SparkFun_APDS9960.h>
#include <Wire.h>
#include <esp_log.h>
#include <functional>
#include "MovingAverage.h"

#define DEBUG_GESTMGR 	1

#if DEBUG_GESTMGR
#define GESTMGR_DEBUG_PRINT(...)   	ESP_LOGI("GESTMGR", __VA_ARGS__); //ESP_LOGD("GESTMGR", __VA_ARGS__);
#else
#define GESTMGR_DEBUG_PRINT(...)
#endif

#define GESTMGR_INFO_PRINT(...)  	ESP_LOGI("GESTMGR", __VA_ARGS__);

// Constants
#define APDS9960_INT_GPIO 			34 //5
#define APDS9960_PROX_INT_HIGH   	30 // Proximity level for interrupt
#define APDS9960_PROX_INT_LOW    	0  // No far interrupt

#define GESTMGR_PROX_MAX_VAL		255
#define GESTMGR_PROX_LOCK_THRES		20//30
#define GESTMGR_PROX_LOCK_INTERVAL 	2000

class GestureManagerClass {
public:

	typedef std::function<void(int gestureDir)> GestureDetectedCallback;
	typedef std::function<void(uint8_t level)> ProximityCallback;

	GestureManagerClass();
	virtual ~GestureManagerClass();

	bool begin();
	void run();
	void runAsync();
	bool setGestureRecognitionEnabled(bool enabled);
	bool setProximityDetectionEnabled(bool enabled);

	bool isGestureRecognitionEnabled() {
		return gestureRecognitionEnabled_;
	}

	bool isProximityDetectionEnabled() {
		return proximityDetectionEnabled_;
	}

	void onGestureDetected(GestureDetectedCallback cb) {
		gestureDetectedCallback_ = cb;
	}

	void onProximityDetected(ProximityCallback cb) {
		proximityDetectedCallback_ = cb;
	}

	void onProximityChangedDetected(ProximityCallback cb) {
		proximityChangedCallback_ = cb;
	}

	void onProximityLockedDetected(ProximityCallback cb) {
		proximityLockedCallback_ = cb;
	}

	SparkFun_APDS9960 *getSensorPtr() {
		return apds_;
	}

private:
	SparkFun_APDS9960 *apds_ = NULL;

	GestureDetectedCallback gestureDetectedCallback_ = NULL;
	ProximityCallback proximityDetectedCallback_ = NULL, proximityLockedCallback_, proximityChangedCallback_;
	bool gestureRecognitionEnabled_ = false, proximityDetectionEnabled_ = false;
	MovingAverage<uint16_t, 5> averageProximity_;

	uint8_t lastProximityLevelLock_ = 0;
	unsigned long lastProximityLockCheckTime_ = 0;

	void handleGesture();
	void processProximityData(uint8_t prox);
};

extern GestureManagerClass GestureManager;

#endif /* COMPONENTS_MAGICBUTTON_SRC_GESTUREMANAGER_H_ */
