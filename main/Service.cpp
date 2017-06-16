/*
 * Service.cpp
 *
 *  Created on: Jun 10, 2017
 *      Author: andri
 */

#include "Service.h"

RgbLedColor_t rgbMagentaColor(200, 20, 80);
RgbLedColor_t rgbBlueColor(10, 10, 190);

Service::Service(AppSettingStorage &appSetting):
		Task(1, "svcTask", 4096, 3),
		appSetting_(appSetting){

}

Service::~Service() {

}

void Service::begin() {

}

void Service::run() {

}

//void Service::doSomethingAboutWaterLevel(uint16_t percent) {
//
//	//uint16_t thresh = appSetting.props.attr.alertThreshold == 0? 20: appSetting.props.attr.alertThreshold;
//	uint16_t thresh = 10;
//
//	if (ledBar_ && !ledBar_->isAnimating()) {
//		//ledBar_->setPercentage(percent);
//		ledBar_->setPercentage(100 - percent);
//		ledBar_->setBlink((boolean) (percent <= thresh));
//	}
//
//	appSetting_.stuff.attr.waterLevelPercent = percent;
//
//	if (this->telemetryDataQueue_) {
//		xQueueSendToBack(telemetryDataQueue_, &appSetting_.stuff.attr, 0);
//	}
//}

void Service::subscribeTelemetryData(xQueueHandle* queue_ptr) {
	if (*queue_ptr == NULL) {
		*queue_ptr = xQueueCreate(10, sizeof(StuffAttr_t));
	}

	this->telemetryDataQueue_ = *queue_ptr;
}

void Service::startServing() {
}

void Service::stopServing() {
}

void Service::handleTouch(uint8_t tNo, touchpad_event_t evt) {

	uint16_t pixelCount = MagicButtonBoard.getRgbLed().getPixelCount();
	uint8_t untilPixelNo = (uint8_t)map((uint8_t)tNo, 0, 7, 0, (pixelCount - 1));

	bool tapped = false;
	if (evt == TOUCHPAD_EVENT_TAP) {
		printf("Touched %d, pixel no: %d\n", tNo, untilPixelNo);
		tapped = true;
	}
	else if (evt == TOUCHPAD_EVENT_PUSH) {
		printf("Push %d, pixel no: %d\n", tNo, untilPixelNo);
	}
	else {
		return;
	}

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

	if (tapped && this->telemetryDataQueue_) {
		appSetting_.stuff.attr.buttonId = tNo;
		strcpy(appSetting_.stuff.attr.requestId, String(esp_random()).c_str());

		xQueueSendToBack(telemetryDataQueue_, &appSetting_.stuff.attr, 0);
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
