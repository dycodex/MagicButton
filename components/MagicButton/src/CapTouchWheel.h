/*
 * CapClickWheel.h
 *
 *  Created on: May 29, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_MAGICBUTTON_SRC_CAPTOUCHWHEEL_H_
#define COMPONENTS_MAGICBUTTON_SRC_CAPTOUCHWHEEL_H_

#define CAP_CLICK_WHEEL_PADS 8

#define TOUCHPAD_FILTER_VALUE   150
#define TOUCHPAD_THRESHOLD  700

#include <Arduino.h>
#include <functional>
#include "Task.h"
#include "touchpad.h"
#include "esp_log.h"
#include <map>

#define CAPWHEEL_DEBUG_PRINT(...)   ESP_LOGI("CAPWHEEL", __VA_ARGS__);
#define CAPWHEEL_INFO_PRINT(...)   	ESP_LOGI("CAPWHEEL", __VA_ARGS__);

class CapTouchWheel;

typedef struct CapTouchWheelTouchEvent {
	touchpad_handle_t handle;
	CapTouchWheel* wheel;
} CapTouchWheelTouchEvent;

class CapTouchWheel: public Task {
public:

	typedef std::function<void(uint8_t touchGpioNo, touchpad_cb_type_t evt)> CapTouchActionCallback;
	typedef std::function<void(int16_t angle, int16_t inc)> CapClickWheelAngleChangedCallback;

	CapTouchWheel();
	virtual ~CapTouchWheel();

	void begin();
	//void calibrate();
	void run();
	void runAsync(void *data);

	void setEnableWheelAngleDetection(bool en) {
		enableWheelAngleDetection_ = en;
	}

	int16_t getWheelAngle(void);
	int16_t getWheelIncrement(void);

	void setWheelAngleChangedCallback(CapClickWheelAngleChangedCallback cb) {
		setEnableWheelAngleDetection(true);
		wheelAngleChangedCallback_ = cb;
	}

	void setTouchActionCallback(CapTouchActionCallback cb) {
		touchActionCallback_ = cb;
	}

private:

	std::map<uint8_t, touchpad_handle_t>  capTouchesMap_;

	uint16_t *baselineVals_ = NULL;
	int16_t wheelDegreeOffset_ = 0;
	int16_t lastWheelAngle_ = 0;

	bool enableWheelAngleDetection_ = false;
	unsigned long lastTouchInterruptedTime_ = 0;

	CapClickWheelAngleChangedCallback wheelAngleChangedCallback_ = NULL;
	CapTouchActionCallback touchActionCallback_ = NULL;

	void registerTouches();
	void touchpadTask();
	static void touchpadInternalCallback(void* arg);
	xQueueHandle xQueueTouchPad_ = NULL;
};

#endif /* COMPONENTS_MAGICBUTTON_SRC_CAPTOUCHWHEEL_H_ */
