/*
 * MagicButton.h
 *
 *  Created on: Apr 26, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_MAGICBUTTON_SRC_MAGICBUTTON_H_
#define COMPONENTS_MAGICBUTTON_SRC_MAGICBUTTON_H_

#include <Arduino.h>
#include <Wire.h>
#include <esp_log.h>
#include <map>
#include "CapTouchWheel.h"
#include "WS2812.h"
//#include "CapacitiveTouch.h"

#define MAGICBUTTON_DEBUG_PRINT(...)   	ESP_LOGD("MAGICBUTTON", __VA_ARGS__);
#define MAGICBUTTON_INFO_PRINT(...)   	ESP_LOGI("MAGICBUTTON", __VA_ARGS__);

#define SDAPIN (GPIO_NUM_21)
#define SCLPIN (GPIO_NUM_22)

#define MAGICBUTTON_RGBLED_GPIO		GPIO_NUM_27
#define MAGICBUTTON_RGBLED_COUNT	15   //Change this for another board type, which has 8 neopixels

class MagicButtonBoardClass {
public:
	MagicButtonBoardClass();
	virtual ~MagicButtonBoardClass();

	void begin();
	void run();
//	void activateTouch(uint8_t gpio, CapacitiveTouch::TouchActionCallback cb, uint16_t touchThreshold = 0);

	void startCapTouchWheel();
	CapTouchWheel& getCapTouchWheel();
	WS2812& getRgbLed();

	void scanI2C();

private:
//	std::map<uint8_t, CapacitiveTouch*>  capTouchesMap_;
	CapTouchWheel *capTouchWheel_ = NULL;
	WS2812 *rgbLed_ = NULL;
};

extern MagicButtonBoardClass MagicButtonBoard;

#endif /* COMPONENTS_MAGICBUTTON_SRC_MAGICBUTTON_H_ */
