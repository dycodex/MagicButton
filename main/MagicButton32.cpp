
/*
 * 1. Open up the project properties
 * 2. Visit C/C++ General > Preprocessor Include Paths, Macros, etc
 * 3. Select the Providers tab
 * 4. Check the box for "CDT GCC Built-in Compiler Settings"
 * 5. Set the compiler spec command to "xtensa-esp32-elf-gcc ${FLAGS} -E -P -v -dD "${INPUTS}""
 * 6. Rebuild the index
*/

extern "C" {
	#include <esp_log.h>
	#include "sdkconfig.h"
	#include <time.h>
	#include <sys/time.h>
	#include "freertos/FreeRTOS.h"
	#include "freertos/task.h"
	#include "esp_deep_sleep.h"
	#include "esp_log.h"
	#include "esp32/ulp.h"
	#include "driver/adc.h"
	#include "driver/gpio.h"
}

#include <string>
#include "WS2812.h"
#include "Arduino.h"
#include "MagicButton.h"
//#include "MB_TestCode.h"
#include "MB_TestI2S.h"
#include "MB_TestCap.h"

const static char *TAG = "APP";

extern "C" {
void app_main(void);
}

// #include "I2SPlayer.h"
// I2SPlayer wavPlayer;

#include "GestureManager.h"

#include "MagicButtonAnimation.h"

RgbLedColor_t fadeColor(200, 20, 80);
//MagicButtonFadeInOutAnimation fadeInOutAnim(MagicButtonBoard.getRgbLed(), fadeColor);
MagicButtonGlowAnimation glowAnim(MagicButtonBoard.getRgbLed(), fadeColor);

RgbLedColor_t myColors[] = { 0xFF0000, 0x00FF00, 0x0000FF };
RgbLedPalette_t paletteRgb = { 3, myColors };
MagicButtonCometAnimation cometAnim(MagicButtonBoard.getRgbLed(), paletteRgb);

MagicButtonArrowAnimation arrowAnim(MagicButtonBoard.getRgbLed(), fadeColor);

// WS2812Color_t fadeColor2(100, 200, 70);
// MagicButtonFadingAnimation  fadingAnim(ws2812, fadeColor2);

// #include "FilesystemHandler.h"

// void initFileSystem(void *p) {
// 	printf("Now initialising the filesystem.\n");
// 	FilesystemHandler *fs_handler = FilesystemHandler::get_instance((char *)"spiffs"/*Mount point */);

// 	if (fs_handler->init_spiffs()) {
// 		printf("Filesystem initted.\n");
// 	}
// }

extern "C" {
	#include <stdlib.h>
	#include <stdio.h>
	#include "esp_vfs.h"
	#include "esp_vfs_fat.h"
	#include "esp_system.h"
	#include "wear_levelling.h"
}

static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

void app_main(void)
{
	initArduino();
	Serial.begin(115200);

	ESP_LOGI(TAG, "It begins!");

	MagicButtonBoard.begin();
	//MagicButton.scanI2C();

	// tryI2SInput(NULL);
	// xTaskCreatePinnedToCore(tryI2SInput, "tryI2SInput", 2048*20, NULL, configMAX_PRIORITIES - 2, NULL, 1);
	// delay(1000);
	// xTaskCreate(initFileSystem, "initFileSystem", 2048*8, NULL, configMAX_PRIORITIES - 2, NULL);

	// 	printf("Now initialising the filesystem.\n");
	// // 	FilesystemHandler *fs_handler = FilesystemHandler::get_instance(0x220000 /*Start address on flash*/,
	// // 								 0x100000  /*Size*/,
	// // 								 (char *)"/spiffs"      /*Mount point */);
	// FilesystemHandler *fs_handler = FilesystemHandler::get_instance((char *)"spiffs"/*Mount point */);

	// 	if (fs_handler->init_spiffs()) {
	// 		printf("Filesystem initted.\n");
	// 	}

	// 	if (fs_handler->init_spiffs()) {
	// //		readWav();
	//
	// 		//SpiffsFile *f = new SpiffsFile();
	// //		wavProperties_t wProps;
	// //		WAVFile *f = new WAVFile();
	// //
	// //		if (f->Open("/musics/T5.wav", &wProps)) {
	// //			Serial.println("File is openned");
	// //
	// //			Serial.printf("audioFormat %d\r\n", wProps.audioFormat);
	// //			Serial.printf("numChannels %d\r\n", wProps.numChannels);
	// //			Serial.printf("sampleRate %d\r\n", wProps.sampleRate);
	// //			Serial.printf("byteRate %d\r\n", wProps.byteRate);
	// //			Serial.printf("blockAlign %d\r\n", wProps.blockAlign);
	// //			Serial.printf("bitsPerSample %d\r\n", wProps.bitsPerSample);
	// //		}
	//
	// 		wavPlayer.init();
	// 		wavPlayer.setVolume(60);
	// 		if (wavPlayer.prepareWav("/musics/happy.wav")) {
	// 			wavPlayer.playAsync();
	// 			//wavPlayer.play();
	// 		}
	// 	}

	//	do_fs_test();

	//trySleep();

	//ws2812FadeInOut(200, 20, 80);
	// xTaskCreate(&ws2812FadeInOutTask, "ws2812FadeInOutTask", 2048, NULL, 5, NULL);

//	fadeInOutAnim.start(2000, 10);
//	glowAnim.start(3000);

	cometAnim.start(2000, ANIM_DIR_RIGHT, 1);

	// Initialize touch pad peripheral
//	touch_pad_init();
	//xTaskCreate(&touchpad_read_task, "touch_pad_read_task", 2048, NULL, 5, NULL);
//	xTaskCreate(&touchpad_circular_task, "touch_pad_read_task", 2048, NULL, 5, NULL);


	MagicButtonBoard.getCapTouchWheel().setTouchActionCallback([](uint8_t tNo) {

		uint16_t pixelCount = MagicButtonBoard.getRgbLed().getPixelCount();
		uint8_t untilPixelNo = (uint8_t)map((uint8_t)tNo, 0, 7, 0, (pixelCount - 1));

		printf("Touched %d, pixel no: %d\n", tNo, untilPixelNo);

		MagicButtonBoard.getRgbLed().clear();
		for(int i = 0; i < pixelCount; i++) {
			if (i > untilPixelNo) {
				break;
			}
			MagicButtonBoard.getRgbLed().setPixel(i, fadeColor);
		}
		MagicButtonBoard.getRgbLed().show();
	});

	// Should explicitly start the cap touch wheel.
	MagicButtonBoard.startCapTouchWheel();

	//	pinMode(39, ANALOG);
	//	pinMode(36, ANALOG);
	//	pinMode(25, OUTPUT); digitalWrite(25, HIGH);
	//	xTaskCreate(&adc1task, "adc1task", 1024*3, NULL, 3, NULL);

	//	xTaskCreate(&tryGesture, "tryGesture", 1024*3, NULL, 3, NULL);

	//	tryI2S();

//	ESP_LOGI(TAG, "Mounting FAT filesystem");
//	// To mount device we need name of device partition, define base_path
//	// and allow format partition in case if it is new one and was not formated before
//	const esp_vfs_fat_mount_config_t mount_config = {
//		.format_if_mount_failed = true,
//		.max_files = 4};
//	esp_err_t err = esp_vfs_fat_spiflash_mount("/spiflash", "spiffs", &mount_config, &s_wl_handle);
//	if (err != ESP_OK)
//	{
//		ESP_LOGE(TAG, "Failed to mount FATFS (0x%x)", err);
//		//return;
//	}

//
//	// delay(50);
//	tryI2SInput(NULL);
//	// initRB();
	
	//xTaskCreate(&consumeI2SInput, "consumeI2SInput", 2048*10, NULL, configMAX_PRIORITIES - 3, NULL);
//	xTaskCreate(&tryI2SInput, "tryI2SInput", 2048*10, NULL, configMAX_PRIORITIES - 2, NULL);

	// xTaskCreatePinnedToCore(&consumeI2SInput , "consumeI2SInput" , 2048*10, NULL, configMAX_PRIORITIES - 3, NULL, 0);
	// xTaskCreatePinnedToCore(&tryI2SInput , "tryI2SInput" , 2048*10, NULL, configMAX_PRIORITIES - 2, NULL, 0);
	
//	xTaskCreate(&tryI2SRecord, "tryI2SRecord", 2048*10, NULL, configMAX_PRIORITIES - 2, NULL);

	//xTaskCreatePinnedToCore(&task_megaphone, "task_megaphone", 16384 * 2, NULL, 20, NULL, 0);

	//	for(;;) {
	//		MagicButton.run();
	//		//ESP_LOGI(TAG, "Touch %d is touched! %d", 27, touchRead(27));
	//		delay(1);
	//	}

	/*
	static uint8_t lastProxLevel = 255;
	static uint8_t reactivateGestureCounter = 0;
	static bool brightLocked = false;
	GestureManager.onProximityLockedDetected([](uint8_t val) {

		if (GestureManager.isGestureRecognitionEnabled()) {
			return;
		}

		if (abs(val - GESTMGR_PROX_MAX_VAL) < GESTMGR_PROX_LOCK_THRES) {
			reactivateGestureCounter++;
			if (reactivateGestureCounter >= 2) {
				reactivateGestureCounter = 0;
				ESP_LOGI(TAG, "Reactivate gesture recog");
				GestureManager.setGestureRecognitionEnabled(true);
			}
		}

		//lastProxLevel = val;
		if (!brightLocked) {
			uint8_t scaled = (uint8_t)map(val, APDS9960_PROX_INT_HIGH, GESTMGR_PROX_MAX_VAL, 0, 100);
			uint8_t from_to[2] = {lastProxLevel, scaled};
			ESP_LOGI(TAG, "LOCKING: Change brightness level from %d to %d", from_to[0], from_to[1]);
//			xTaskCreate(&ws2812FadingAnimTask, "ws2812FadingAnimTask", 2048, (void*)from_to, 2, NULL);

			fadingAnim.start(lastProxLevel, scaled, 100);

			brightLocked = true;
		}
		else {
			brightLocked = false;
		}
	});

	GestureManager.onProximityChangedDetected([](uint8_t val) {

		if (GestureManager.isGestureRecognitionEnabled()) {
			return;
		}
		uint8_t scaled = (uint8_t)map(val, APDS9960_PROX_INT_HIGH, GESTMGR_PROX_MAX_VAL, 0, 100);

		if (!brightLocked) {
			uint8_t from_to[2] = {lastProxLevel, scaled};
			ESP_LOGI(TAG, "TRANS: Change brightness level from %d to %d", from_to[0], from_to[1]);
//			xTaskCreate(&ws2812FadingAnimTask, "ws2812FadingAnimTask", 2048, (void*)from_to, 2, NULL);

			fadingAnim.start(lastProxLevel, scaled, 100);
		}

		lastProxLevel = scaled;
	});
	*/

	GestureManager.onGestureDetected([](int gestDir) {
		ESP_LOGI(TAG, "Yuhuuu... gesture detected %d", gestDir);
 //		xTaskCreate(&ws2812ArrowAnimTask, "ws2812ArrowAnimTask", 2048, (void*)gestDir, 2, NULL);

		if (gestDir == DIR_FAR) {
			glowAnim.start(1000);
		}
		else if (gestDir == DIR_NEAR) {
			glowAnim.start(500);
		}
		else if (gestDir == DIR_NONE) {

		}
		else {
			arrowAnim.animateArrow((MagicButtonAnimationArrow_t)gestDir);
		}

 //		if (gestDir == DIR_NEAR) {
 //			GestureManager.setGestureRecognitionEnabled(false);
 //		}

	});

	if (GestureManager.begin()) {
		GestureManager.runAsync();
	}
}
