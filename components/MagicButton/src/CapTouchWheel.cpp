/*
 * CapClickWheel.cpp
 *
 * Some code is adapted from this: https://github.com/tricorderproject/arducordermini/blob/master/firmware/test2k/Adafruit_MPR121.cpp
 *
 *  Created on: May 29, 2017
 *      Author: andri
 *
 */

#include "CapTouchWheel.h"

extern "C" {
#include "driver/touch_pad.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
}

const size_t calibration_count = 16;
static int8_t lastIntrTouchPad = -1;
volatile bool touchPadInterrupted = false;

const int8_t padGpios[] = {
  4,
  0,
  2,
  15,
  13,
  12,
  14,
  27,
  32,
  33
};

const uint8_t padNos[] = {
  0,
  2,
  3,
  4,
  5,
  6,
  8,
  9
};

CapTouchWheel::CapTouchWheel():
Task("capTouchWheelTask", 4096, configMAX_PRIORITIES - 2){
}

CapTouchWheel::~CapTouchWheel() {
	if (baselineVals_ != NULL) {
		delete baselineVals_;
		baselineVals_ = NULL;
	}

	for (const auto &ctPair : capTouchesMap_) {
		touchpad_delete(ctPair.second);
	}

	capTouchesMap_.clear();
}

/*
void rtc_intr(void * arg)
{
	uint32_t pad_intr = READ_PERI_REG(SENS_SAR_TOUCH_CTRL2_REG) & 0x3ff;
	uint32_t rtc_intr = READ_PERI_REG(RTC_CNTL_INT_ST_REG);
	uint8_t i = 0;

	// clear interrupt
	WRITE_PERI_REG(RTC_CNTL_INT_CLR_REG, rtc_intr);
	SET_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL2_REG, SENS_TOUCH_MEAS_EN_CLR);

	if (rtc_intr & RTC_CNTL_TOUCH_INT_ST) {
		for (i = 0; i < TOUCH_PAD_MAX; ++i) {
			if (i == 7 || i == 1) {
				continue;
			}

			if ((pad_intr >> i) & 0x01) {
				//ets_printf("touch pad intr %u\n",i);
				lastIntrTouchPad = i;
				touchPadInterrupted = true;
			}
		}
	}
}
*/

//void CapTouch_TouchInterruptHandler() {
//	touchPadInterrupted = true;
//}

void CapTouchWheel::begin() {

	CAPWHEEL_DEBUG_PRINT("Starting CapTouchWheel");

	//setStackSize(1536);
	//setPriority(configMAX_PRIORITIES - 2);

	baselineVals_ = new uint16_t[TOUCH_PAD_MAX-1];

	touch_pad_init();
//	calibrate();
//	touch_pad_isr_handler_register(rtc_intr, NULL, 0, NULL);

	registerTouches();
}

void CapTouchWheel::registerTouches() {

	touchpad_handle_t touchpad_dev;
	xQueueTouchPad_ = NULL;

	for (int pad = 0; pad < TOUCH_PAD_MAX; pad++) {
		if (pad == 7 || pad == 1) {
			baselineVals_[pad] = 0;
			continue;
		}

		int avgVal = 0;
		for (int i = 0; i < calibration_count; ++i) {
			uint16_t touch_value;
			touch_pad_read((touch_pad_t)pad, &touch_value);
			avgVal += touch_value;
		}
		avgVal /= calibration_count;
		baselineVals_[pad] = avgVal;
		int threshold = (pad == 8)? TOUCHPAD_THRESHOLD: baselineVals_[pad] - 50; //100;
		//int threshold = TOUCHPAD_THRESHOLD;

		//touch_pad_config((touch_pad_t)pad, threshold);

		size_t idx = 0;
		while ( idx < 8 && padNos[idx] != pad ) ++idx;
		idx = ( idx == 8 ? -1 : idx );
		if (idx == -1) {
			CAPWHEEL_DEBUG_PRINT("FUCK!");
			break;
		}

		CAPWHEEL_DEBUG_PRINT("Registering pad %d (%d) with threshold %d", pad, pad, threshold);

		touchpad_dev = touchpad_create((touch_pad_t)pad, threshold, TOUCHPAD_FILTER_VALUE, TOUCHPAD_SINGLE_TRIGGER, 5, &xQueueTouchPad_, 20);

		capTouchesMap_[idx] = touchpad_dev;
	}
}

void CapTouchWheel::touchpadTask() {

	portBASE_TYPE xStatus;
	touchpad_msg_t recv_value;
	uint16_t value;

	if (xQueueTouchPad_ != NULL) {
		xStatus = xQueueReceive(this->xQueueTouchPad_, &recv_value, 0);//portMAX_DELAY);
		if (xStatus == pdPASS) {
			CAPWHEEL_DEBUG_PRINT("number of touch pad:%d", recv_value.num);
//			switch (recv_value.event) {
//				case TOUCHPAD_EVENT_TAP:
//					CAPWHEEL_DEBUG_PRINT("touch pad event tap");
//					break;
//				case TOUCHPAD_EVENT_LONG_PRESS:
//					CAPWHEEL_DEBUG_PRINT("touch pad event long press");
//					break;
//				case TOUCHPAD_EVENT_PUSH:
//					CAPWHEEL_DEBUG_PRINT("touch pad event push");
//					break;
//				case TOUCHPAD_EVENT_RELEASE:
//					CAPWHEEL_DEBUG_PRINT("touch pad event release");
//					break;
//				default:
//					break;
//			}

			touchpad_handle_t th = recv_value.handle;
			int idx = -1;
			for (auto &i : this->capTouchesMap_) {
			  if (i.second == th) {
				 idx = i.first;
				 break; // to stop searching
			  }
			}

			if (idx > -1) {
				touchActionCallback_(idx, recv_value.event);
			}
		}
	}
	else {
		CAPWHEEL_DEBUG_PRINT("touch pads are all deleted");
		vTaskDelete(NULL);
	}
}

/*
void CapTouchWheel::calibrate() {
	//int i = 0;

	//for (int pad = (TOUCH_PAD_MAX - 1); pad >= 0; pad--) {
	for (int pad = 0; pad < TOUCH_PAD_MAX; pad++) {
		if (pad == 7 || pad == 1) {
			baselineVals_[pad] = 0;
			continue;
		}

		int avgVal = 0;

//		int8_t touchGpioNo = padGpios[pad];

		for (int i = 0; i < calibration_count; ++i) {
			uint16_t touch_value;
			touch_pad_read((touch_pad_t)pad, &touch_value);
//			touch_value = touchRead(touchGpioNo);
			avgVal += touch_value;
		}
		avgVal /= calibration_count;

		baselineVals_[pad] = avgVal;

		int threshold = baselineVals_[pad] - 100;
		touch_pad_config((touch_pad_t)pad, threshold);
//		touchAttachInterrupt(touchGpioNo, CapTouch_TouchInterruptHandler, threshold);
//		i++;
	}
}
*/

void CapTouchWheel::runAsync(void* data) {
	for(;;) {

		if (enableWheelAngleDetection_) {
			//getWheelAngle();
			if (wheelAngleChangedCallback_) {
				int16_t inc = getWheelIncrement();
				if (lastWheelAngle_ > -1) {
					wheelAngleChangedCallback_(lastWheelAngle_, inc);
				}
			}
		}

		/*
		if (touchPadInterrupted) {
			touchPadInterrupted = false;

			if ((millis() - lastTouchInterruptedTime_) > 200) {

				uint16_t tval;
				touch_pad_read((touch_pad_t)lastIntrTouchPad, &tval);
				int16_t threshold = (baselineVals_[lastIntrTouchPad] - 100);
				//make sure it's the right pad
				if (tval <= threshold) {

					lastTouchInterruptedTime_ = millis();

					//callback here
					if (touchActionCallback_) {

						size_t idx = 0;
						while ( idx < 8 && padNos[idx] != lastIntrTouchPad ) ++idx;
						idx = ( idx == 8 ? -1 : idx );

						//printf("Touch = %d\n", idx);//lastIntrTouchPad);
						touchActionCallback_(idx);
					}
				}
			}
		}
		*/

		touchpadTask();

		delay(50);
	}
}

void CapTouchWheel::run(void) {
}

int16_t CapTouchWheel::getWheelAngle(void) {

	int16_t wheelDeltas[CAP_CLICK_WHEEL_PADS];

	uint8_t i = 0;
	uint16_t touch_value;

	for (int pad = 0; pad < TOUCH_PAD_MAX; pad++) {
		if (pad == 7 || pad == 1) {
			continue;
		}

		touch_pad_read((touch_pad_t)pad, &touch_value);
//		touch_value = touchRead(padGpios[pad]);

		int diff = baselineVals_[pad] - touch_value;
		wheelDeltas[i] = diff;
		i++;
	}

	int16_t maxVal = 0;
	int16_t maxIdx = -1;
//	Serial.print("Delta = ");
	for (i = 0; i < CAP_CLICK_WHEEL_PADS; i++) {
//		Serial.printf("%d, ", wheelDeltas[i]);
		if (wheelDeltas[i] > maxVal) {
			maxVal = wheelDeltas[i];
			maxIdx = i;
		}
	}

//	Serial.printf("\nMax = %d %d\n", maxIdx, maxVal);

	if (maxVal < 80) {    // was <20
//		Serial.println("FCUK");
		return -1;
	}


	// Find neighbouring values
	int16_t v1Idx = (maxIdx-1);
	if (v1Idx < 0) v1Idx += CAP_CLICK_WHEEL_PADS; // Handle the negitive modulo

	int16_t v1 = wheelDeltas[v1Idx];                   // Pad before
	int16_t v2 = wheelDeltas[maxIdx];                  // Pad with highest value
	int16_t v3 = wheelDeltas[(maxIdx+1) % CAP_CLICK_WHEEL_PADS];  // Pad after
	float sum = v1 + v2 + v3;

	// Proportions
	float c1 = v1 / sum;
	float c2 = v2 / sum;
	float c3 = v3 / sum;

	// Offset
	// float offset = c2 + c3;                // Offset (-0.5 .. +0.5)
	float offset = 0;
	if (c1 > c3) {
		offset = -c1;
	} else {
		offset = c3;
	}
	float offsetPad = (float) (maxIdx) + offset; // Offset relative to pad (e.g. for a 8 pad wheel, 0-7, representing 0-360 degrees)
	if (offsetPad < 0)
		offsetPad += CAP_CLICK_WHEEL_PADS; // Bound check incase we go negitive for half of the first wheel

	// Calculate the touch position (in degrees)
	int16_t degree = floor((offsetPad / CAP_CLICK_WHEEL_PADS) * 360.0f) - wheelDegreeOffset_;//-20; //-20 = offset
	if (degree < 0)
		degree += 360;
	if (degree > 360)
		degree -= 360;

//	Serial.printf("Degree = %d\n", degree);

	return degree;
}

int16_t CapTouchWheel::getWheelIncrement(void) {

	int16_t curWheelAngle = getWheelAngle();
	int16_t wheelAnglePerTick = 45;//90;

	// Case 1: Wheel is not being touched)
	if (curWheelAngle == -1) {
		lastWheelAngle_ = curWheelAngle;
		return 0;
	}

	// Case 2: Wheel has just started being touched
	if ((lastWheelAngle_ == -1) && (curWheelAngle > -1)) {
		lastWheelAngle_ = curWheelAngle;
		return 0;
	}

	// Case 3: Wheel was touched previously, and is currently being touched
	// Special case: if current wheel angle and previous angle differ by more than 180, then a large move was made, or we're straddling
	// the 359/0 boundary. Let's make the math easier.
	int16_t curWheelAngleAdj = curWheelAngle;
	if (curWheelAngleAdj < (lastWheelAngle_ - 180)) {
		curWheelAngleAdj += 360;
	} else if (curWheelAngleAdj > (lastWheelAngle_ + 180)) {
		curWheelAngleAdj -= 360;
	}

	//CAPWHEEL_DEBUG_PRINT("   lastWheelAngle: %#x", lastWheelAngle_);
	CAPWHEEL_DEBUG_PRINT("   lastWheelAngle: %d", lastWheelAngle_);
	CAPWHEEL_DEBUG_PRINT("   curWheelAngleAdj: %d", curWheelAngleAdj);

	int16_t delta = (curWheelAngleAdj - lastWheelAngle_) / wheelAnglePerTick;
	CAPWHEEL_DEBUG_PRINT("   delta: %d", delta);

	if (delta != 0) {
		// If we've made a movement that counts for at least one tick, then update lastWheelAngle to reflect the new position of the pointer
		lastWheelAngle_ = curWheelAngle;
	}

	// Return the number of inrement/decrement moves we've made  (or 0 if we haven't made any)
	return delta;
}
