/*
 * GestureManager.cpp
 *
 *  Created on: May 12, 2017
 *      Author: andri
 */

#include "GestureManager.h"


volatile bool isr_flag_ = 0;

GestureManagerClass::GestureManagerClass():
		Task("gestureManagerTask", 4096, 2){
}

GestureManagerClass::~GestureManagerClass() {
	if (apds_ != NULL) {
		delete apds_;
		apds_ = NULL;
	}
}

void sensorInterrupted() {
	isr_flag_ = true;
}

bool GestureManagerClass::begin() {

#if APDS9960_INT_ACTIVE_HIGH
	pinMode(APDS9960_INT_GPIO, INPUT);

	// Initialize interrupt service routine
	attachInterrupt(APDS9960_INT_GPIO, sensorInterrupted, RISING);
#else
	pinMode(APDS9960_INT_GPIO, INPUT_PULLUP);
	attachInterrupt(APDS9960_INT_GPIO, sensorInterrupted, FALLING);
#endif


	apds_ = new SparkFun_APDS9960();

#if DEBUG_GESTMGR
	printf("\n");

	GESTMGR_DEBUG_PRINT("---------------------------------------");
	GESTMGR_DEBUG_PRINT("Gesture Manager");
	GESTMGR_DEBUG_PRINT("---------------------------------------");

#endif

	// Initialize APDS-9960 (configure I2C and initial values)
	if (apds_->init()) {
		GESTMGR_DEBUG_PRINT("Gesture Manager initialization complete");
	} else {
		GESTMGR_DEBUG_PRINT("Something went wrong during Gesture Manager init!");

		goto failed;
	}

	if (!setProximityDetectionEnabled(true)) {
		goto failed;
	} else {
		averageProximity_.init(APDS9960_PROX_INT_HIGH);
	}

	if (!setGestureRecognitionEnabled(true)) {
		GESTMGR_DEBUG_PRINT("Something went wrong when enabling gesture sensor!");
		goto failed;
	}

	return true;

failed: {
		delete apds_;
		apds_ = NULL;

		detachInterrupt(APDS9960_INT_GPIO);
		return false;
	}

}

void GestureManagerClass::processProximityData(uint8_t prox) {
	uint8_t avgProx = averageProximity_.add(prox);

	if (proximityDetectedCallback_) {
		proximityDetectedCallback_(avgProx);
	}

	//detect level locking
	if (abs(avgProx - lastProximityLevelLock_) < GESTMGR_PROX_LOCK_THRES) {

		if ((millis() - lastProximityLockCheckTime_) > GESTMGR_PROX_LOCK_INTERVAL) {
			//Locked
//			lastProximityLevelLock_ = avgProx;
			lastProximityLockCheckTime_ = millis();

			if (proximityLockedCallback_) {
				proximityLockedCallback_(lastProximityLevelLock_);
			}

			GESTMGR_DEBUG_PRINT("Proximity locked at: %d!", lastProximityLevelLock_);
		}
	}
	else {
		//significant change
		GESTMGR_DEBUG_PRINT("Proximity significantly changed from %d to %d", lastProximityLevelLock_, avgProx);

		lastProximityLevelLock_ = avgProx;
		lastProximityLockCheckTime_ = millis();

		if (proximityChangedCallback_) {
			proximityChangedCallback_(avgProx);
		}
	}
}

void GestureManagerClass::run() {

	if (isr_flag_ && apds_ != NULL) {

		//GESTMGR_DEBUG_PRINT("PROX INTERRUPTED");

		if (apds_->isInterrupted(INT_GESTURE)) {
			GESTMGR_DEBUG_PRINT("GESTURE INTERRUPTED");
		}

		if (apds_->isInterrupted(INT_PROXIMITY)) {
			GESTMGR_DEBUG_PRINT("PROX INTERRUPTED");
		}

		if (proximityDetectionEnabled_ && apds_->isInterrupted(INT_PROXIMITY)) {
//			GESTMGR_DEBUG_PRINT("PROX INTERRUPTED");
			// Read proximity level and print it out
			uint8_t currProximityLevel_ = 0;
			if (!apds_->readProximity(currProximityLevel_)) {
				GESTMGR_DEBUG_PRINT("Error reading proximity value");
			} else {

//				GESTMGR_DEBUG_PRINT("Proximity detected! Level: %d", currProximityLevel_);
//				if (proximityDetectedCallback_) {
//					proximityDetectedCallback_(currProximityLevel_);
//				}

				processProximityData(currProximityLevel_);
			}

			// Reset flag and clear APDS-9960 interrupt (IMPORTANT!)
			if (!apds_->clearProximityInt()) {
				GESTMGR_DEBUG_PRINT("Error clearing interrupt");
			}
		}

		if (gestureRecognitionEnabled_ && apds_->isInterrupted(INT_GESTURE)) {
			GESTMGR_DEBUG_PRINT("GESTURE INTERRUPTED");
			detachInterrupt(APDS9960_INT_GPIO);

			handleGesture();
#if APDS9960_INT_ACTIVE_HIGH
			attachInterrupt(APDS9960_INT_GPIO, sensorInterrupted, RISING);
#else
			attachInterrupt(APDS9960_INT_GPIO, sensorInterrupted, FALLING);
#endif
		}

		isr_flag_ = false;
	}
}

//void task_function_run(void *pvParameter)
//{
//	for(;;) {
//		((GestureManagerClass*)pvParameter)->run();
//		delay(1);
//	}
//
//	vTaskDelete(NULL);
//}
//
//void GestureManagerClass::runAsync() {
//	TaskHandle_t xHandle = NULL;
//	xTaskCreate(&task_function_run, "GestureManagerTask", 4096, this, 2, &xHandle);
//}

void GestureManagerClass::runAsync(void* data) {
	for(;;) {
		run();
		delay(1);
	}
}

bool GestureManagerClass::setProximityDetectionEnabled(bool enabled) {

	bool ret = true;

	if (enabled) {
		// Adjust the Proximity sensor gain
		if (!apds_->setProximityGain(PGAIN_2X)) {
			GESTMGR_DEBUG_PRINT("Something went wrong trying to set PGAIN");
		}

		// Set proximity interrupt thresholds
		if (!apds_->setProximityIntLowThreshold(APDS9960_PROX_INT_LOW)) {
			GESTMGR_DEBUG_PRINT("Error writing low threshold");
		}
		if (!apds_->setProximityIntHighThreshold(APDS9960_PROX_INT_HIGH)) {
			GESTMGR_DEBUG_PRINT("Error writing high threshold");
		}

		// Start running the APDS-9960 proximity sensor (interrupts)
		if (apds_->enableProximitySensor(true)) {
			GESTMGR_DEBUG_PRINT("Proximity sensor is now running");

		} else {
			GESTMGR_DEBUG_PRINT("Something went wrong during sensor init!");
			ret = false;
		}
	}
	else {
		if (!apds_->disableProximitySensor()) {
			GESTMGR_DEBUG_PRINT("Proximity sensor is failed to disable");
			ret = false;
		}
	}

	proximityDetectionEnabled_ = enabled;
	return ret;
}

bool GestureManagerClass::setGestureRecognitionEnabled(bool enabled) {

	bool ret = true;

	if (enabled) {
		// Start running the APDS-9960 gesture sensor engine
		if (apds_->enableGestureSensor(true)) {
			GESTMGR_DEBUG_PRINT("Gesture sensor is now running");
			ret = true;
		} else {
			GESTMGR_DEBUG_PRINT("Something went wrong during gesture sensor init!");
			ret = false;
		}
	}
	else if (gestureRecognitionEnabled_) {
		if (apds_->disableGestureSensor()) {
			GESTMGR_DEBUG_PRINT("Gesture sensor is now disabled");
			ret = true;
		} else {
			GESTMGR_DEBUG_PRINT("Something went wrong when disabling gesture sensor!");
			ret = false;
		}
	}

	gestureRecognitionEnabled_ = enabled;
	return ret;
}

void GestureManagerClass::handleGesture() {

	if (apds_->isGestureAvailable()) {

		int gest = apds_->readGesture();

#if DEBUG_GESTMGR
		switch (gest) {
		case DIR_UP:
			Serial.println("UP");
			break;
		case DIR_DOWN:
			Serial.println("DOWN");
			break;
		case DIR_LEFT:
			Serial.println("LEFT");
			break;
		case DIR_RIGHT:
			Serial.println("RIGHT");
			break;
		case DIR_NEAR:
			Serial.println("NEAR");
			break;
		case DIR_FAR:
			Serial.println("FAR");
			break;
		default:
			Serial.println("NONE");
			break;
		}
#endif

		if (gestureDetectedCallback_) {
			gestureDetectedCallback_(gest);
		}
	}
}


GestureManagerClass GestureManager;

