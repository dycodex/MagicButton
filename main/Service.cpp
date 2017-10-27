/*
 * Service.cpp
 *
 *  Created on: Jun 10, 2017
 *      Author: andri
 */

#include "Service.h"


RgbLedColor_t rgbMagentaColor(200, 20, 80);
RgbLedColor_t rgbBlueColor(10, 10, 190);

//RgbLedColor_t glowColor_(10, 10, 190);
//MagicButtonGlowAnimation glowAnimation_(MagicButtonBoard.getRgbLed(), glowColor_);

Service::Service(AppSettingStorage &appSetting):
		Task(1, "svcTask", 4096, 3),
		appSetting_(appSetting){

}

Service::~Service() {
	if (requestDataQueue_ != 0) {
		vQueueDelete(requestDataQueue_);
	}
}

void Service::begin() {
	if (this->responseDataQueue == 0) {
		this->responseDataQueue = xQueueCreate(5, sizeof(std::string));
	}
}

void Service::run() {

	if (this->responseDataQueue != 0) {
		std::string jsonString;
		if (xQueueReceive(this->responseDataQueue, &jsonString, 1000/portTICK_PERIOD_MS) == pdTRUE) {
			handleResponse(jsonString);
		}
	}
}

void Service::subscribeForRequestData(xQueueHandle* queue_ptr) {
//	if (*queue_ptr == 0) {
//		*queue_ptr = xQueueCreate(10, sizeof(StuffAttr_t));
//	}

//	this->requestDataQueue_ = *queue_ptr;

	if (this->requestDataQueue_ == 0) {
		this->requestDataQueue_ = xQueueCreate(10, sizeof(StuffAttr_t));
	}

	*queue_ptr = this->requestDataQueue_;
}

void Service::notifyResponse(std::string &jsonString) {
	xQueueSendToBack(this->responseDataQueue, &jsonString, 0);
}

void Service::startServing() {
}

void Service::stopServing() {
}

void Service::handleResponse(std::string& jsonString) {
//	//parse json
//	SVC_DEBUG_PRINT("GOT RESPONSE FROM QUEUE: %s", jsonString.c_str());
//
//	JsonObject json = JSON::parseObject(jsonString);
//	auto indicatorString = json.getString("indicator");
//	//SVC_DEBUG_PRINT("INDICATOR: %s", indicatorString.c_str());
//
//	unsigned long indicatorLong = strtol(indicatorString.c_str(), NULL, 16);
//	SVC_DEBUG_PRINT("INDICATOR: %lx", indicatorLong);
//	if (indicatorLong > 0) {
//
////		if (cometPalleteRgb.colors != NULL) {
////			delete cometPalleteRgb.colors;
////			cometPalleteRgb.colors = NULL;
////		}
//
//		glowColor_.setColor(indicatorLong);
//		//glowAnimation_.setColor(col);
//
//		glowAnimation_.start(2000);
//	}
//
//	JSON::deleteObject(json);
}

void Service::handleTouch(uint8_t tNo, touchpad_cb_type_t evt) {
	uint16_t pixelCount = MagicButtonBoard.getRgbLed().getPixelCount();
	uint8_t untilPixelNo = (uint8_t)map((uint8_t)tNo, 0, 7, 0, (pixelCount - 1));

	// DEFAULT TO TAP
	bool tapped = true;

	MagicButtonBoard.getRgbLed().clear();
//		for(int i = 0; i < pixelCount; i++) {
//			if (i > untilPixelNo) {
//				break;
//			}
//			MagicButtonBoard.getRgbLed().setPixel(i, (tapped? blueColor: fadeColor));
//		}
	MagicButtonBoard.getRgbLed().setPixel(untilPixelNo, (tapped? rgbBlueColor: rgbMagentaColor));
	MagicButtonBoard.getRgbLed().show();

	//now do publish
	if (tapped && this->requestDataQueue_) {

		appSetting_.stuff.attr.buttonId = tNo;
		strcpy(appSetting_.stuff.attr.requestId, String(esp_random()).c_str());

		xQueueSendToBack(requestDataQueue_, &appSetting_.stuff.attr, 0);

		vTaskDelay(2000/portTICK_PERIOD_MS);
		MagicButtonBoard.getRgbLed().clear();
		MagicButtonBoard.getRgbLed().show();
	}
}

void Service::runAsync(void* taskData) {

	using namespace std::placeholders;
	MagicButtonBoard.getCapTouchWheel().setTouchActionCallback(std::bind(&Service::handleTouch, this, _1, _2));
	MagicButtonBoard.startCapTouchWheel();

	while(1) {
		run();
		vTaskDelay(1/portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}


