/*
 * MagicButton.cpp
 *
 *  Created on: Apr 26, 2017
 *      Author: andri
 */

#include "MagicButton.h"
#include "driver/touch_pad.h"

MagicButtonBoardClass::MagicButtonBoardClass() {
	// TODO Auto-generated constructor stub

}

MagicButtonBoardClass::~MagicButtonBoardClass() {
//	for (const auto &ctPair : capTouchesMap_) {
//		delete ctPair.second;
//	}
//	capTouchesMap_.clear();
	if (capTouchWheel_ != NULL) {
		delete capTouchWheel_;
		capTouchWheel_ = NULL;
	}

	if (rgbLed_ != NULL) {
		delete rgbLed_;
		rgbLed_ = NULL;
	}
}

void MagicButtonBoardClass::begin() {
}

void MagicButtonBoardClass::run() {

	//Handling cap touches
//	for (const auto &ctPair : capTouchesMap_) {
//		ctPair.second->run();
//	}

}

void MagicButtonBoardClass::startCapTouchWheel() {

	if (capTouchWheel_ == NULL) {
		capTouchWheel_ = new CapTouchWheel();
		capTouchWheel_->begin();
	}

	capTouchWheel_->setEnableWheelAngleDetection(true);
//	capTouchWheel_->setWheelAngleChangedCallback([](int16_t angle, int16_t inc) {
//		printf("Angle %d, inc %d\n", angle, inc);
//	});
//	capTouchWheel_->setTouchActionCallback([](uint8_t tNo) {
//		printf("Touched %d\n", tNo);
//	});

	capTouchWheel_->start(NULL);
}

CapTouchWheel& MagicButtonBoardClass::getCapTouchWheel() {
	if (capTouchWheel_ == NULL) {
		capTouchWheel_ = new CapTouchWheel();
		capTouchWheel_->begin();
	}

	return *capTouchWheel_;
}

WS2812& MagicButtonBoardClass::getRgbLed() {
	if (rgbLed_ == NULL) {
		rgbLed_ = new WS2812(
			MAGICBUTTON_RGBLED_GPIO, // Pin
			MAGICBUTTON_RGBLED_COUNT // Pixel count
		);
	}

	return *rgbLed_;
}

//void MagicButtonBoardClass::activateTouch(uint8_t gpio, CapacitiveTouch::TouchActionCallback cb, uint16_t touchThreshold) {
//	if (capTouchesMap_.find(gpio) == capTouchesMap_.end()) {
//		CapacitiveTouch *ct = new CapacitiveTouch(gpio, touchThreshold);
//		ct->onTouched(cb);
//		ct->begin();
//
//		capTouchesMap_[gpio] = ct;
//	}
//	else {
//		//capTouchesMap_[gpio]->onTouched(cb);
//	}
//}

/**** UTILILTIES ****/
void MagicButtonBoardClass::scanI2C() {

	MAGICBUTTON_INFO_PRINT("I2C scanning with SDA=%d, CLK=%d", SDAPIN, SCLPIN);
	Wire.begin(SDAPIN, SCLPIN);
	int address;
	int foundCount = 0;
	for (address=1; address<127; address++) {
		Wire.beginTransmission(address);
		uint8_t error = Wire.endTransmission();
		if (error == 0) {
			foundCount++;
			MAGICBUTTON_INFO_PRINT("Found device at 0x%.2x", address);
		}
	}
	MAGICBUTTON_INFO_PRINT("Found %d I2C devices by scanning.", foundCount);

}

MagicButtonBoardClass MagicButtonBoard;
