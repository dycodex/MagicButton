/*
 * CapacitiveTouch.h
 *
 *  Created on: Apr 21, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_CONNIO_SRC_CAPACITIVETOUCH_H_
#define COMPONENTS_CONNIO_SRC_CAPACITIVETOUCH_H_

#include <Arduino.h>

// #undef min
// #undef max
#include <functional>

class CapacitiveTouch {
public:

	typedef std::function<void(uint8_t touchGpioNo)> TouchActionCallback;

	CapacitiveTouch(uint8_t touchGpioNo, uint16_t touchThreshold = 0);
	virtual ~CapacitiveTouch();

	void begin();
	void run();

//	static CapacitiveTouch *GetInstance()
//	{
//		return CapacitiveTouch::capTouchPtr_;
//	}

	void onTouched(TouchActionCallback cb) {
		touchCallback_ = cb;
	}

//	void setTouchInterrupted(bool inter) {
//		touchInterrupted_ = inter;
//	}

	uint8_t getGpioNo() {
		return touchGpioNo_;
	}

	void calibrate();
private:
	//static CapacitiveTouch *capTouchPtr_;
	uint8_t touchGpioNo_;
	uint16_t touchThreshold_;
	//volatile bool touchInterrupted_ = false;

	TouchActionCallback touchCallback_ = NULL;
	unsigned long capTouch_LastTouchInterruptedTime_ = 0;
};

#endif /* COMPONENTS_CONNIO_SRC_CAPACITIVETOUCH_H_ */
