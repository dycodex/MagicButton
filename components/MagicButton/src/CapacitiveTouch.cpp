/*
 * CapacitiveTouch.cpp
 *
 *  Created on: Apr 21, 2017
 *      Author: andri
 */

#include "CapacitiveTouch.h"

//CapacitiveTouch *CapacitiveTouch::capTouchPtr_ = NULL;
static volatile bool capTouch_TouchInterrupted_ = false;
//static volatile unsigned long capTouch_LastTouchInterruptedTime_ = 0;

CapacitiveTouch::CapacitiveTouch(uint8_t touchGpioNo, uint16_t touchThreshold):
touchGpioNo_(touchGpioNo), touchThreshold_(touchThreshold){

}

CapacitiveTouch::~CapacitiveTouch() {

}

void CapTouch_TouchInterruptHandler() {
	capTouch_TouchInterrupted_ = true;

//	CapacitiveTouch *capTouchPtr = CapacitiveTouch::GetInstance();
//	capTouchPtr->setTouchInterrupted(true);
}

void CapacitiveTouch::begin() {
	//CapacitiveTouch::capTouchPtr_ = this;
	//Serial.printf("Beginning cap touch for gpio %d\n", touchGpioNo_);

	if (touchThreshold_ > 0) {
		touchAttachInterrupt(touchGpioNo_, CapTouch_TouchInterruptHandler, touchThreshold_);
	}
	else {
		calibrate();
	}
}

void CapacitiveTouch::run() {

	if (capTouch_TouchInterrupted_ && (touchRead(touchGpioNo_) <= touchThreshold_)) {
		capTouch_TouchInterrupted_ = false;

		if ((millis() - capTouch_LastTouchInterruptedTime_) > 200 && touchCallback_) {
			capTouch_LastTouchInterruptedTime_ = millis();
			touchCallback_(touchGpioNo_);
		}
	}
}

void CapacitiveTouch::calibrate() {

	printf("Touch pad calibrating...\n");

	uint16_t th = 1000;

	int avg = 0;
	const size_t calibration_count = 128;
	for (int i = 0; i < calibration_count; ++i) {
		uint16_t val = touchRead(touchGpioNo_);
		avg += val;
	}
	avg /= calibration_count;
	const int min_reading = 50;//300;

	if (avg < min_reading) {
		printf("Touch pad #%d average reading is too low: %d (expecting at least %d). "
			   "Not using for deep sleep wakeup.\n", touchGpioNo_, avg, min_reading);
		th = 0;

	} else {
		int threshold = avg;// - 100;
		printf("Touch pad #%d average: %d, wakeup threshold set to %d.\n", touchGpioNo_, avg, threshold);
		th = threshold;
	}

	if (th > 0) {
		touchThreshold_ = th;
		touchAttachInterrupt(touchGpioNo_, CapTouch_TouchInterruptHandler, touchThreshold_);
	}
}
