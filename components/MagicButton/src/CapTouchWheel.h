/*
 * CapClickWheel.h
 *
 *  Created on: May 29, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_MAGICBUTTON_SRC_CAPTOUCHWHEEL_H_
#define COMPONENTS_MAGICBUTTON_SRC_CAPTOUCHWHEEL_H_

#define CAP_CLICK_WHEEL_PADS 8

#include <Arduino.h>
#include <functional>
#include "Task.h"

class CapTouchWheel: public Task {
public:

	typedef std::function<void(uint8_t touchGpioNo)> CapTouchActionCallback;
	typedef std::function<void(int16_t angle, int16_t inc)> CapClickWheelAngleChangedCallback;

	CapTouchWheel();
	virtual ~CapTouchWheel();

	void begin();
	void calibrate();
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
	uint16_t *baselineVals_ = NULL;
	int16_t wheelDegreeOffset_ = 0;
	int16_t lastWheelAngle_ = 0;

	bool enableWheelAngleDetection_ = true;
	unsigned long lastTouchInterruptedTime_ = 0;

	CapClickWheelAngleChangedCallback wheelAngleChangedCallback_ = NULL;
	CapTouchActionCallback touchActionCallback_ = NULL;
};

#endif /* COMPONENTS_MAGICBUTTON_SRC_CAPTOUCHWHEEL_H_ */
