/*
 * MB_TestCode.h
 *
 *  Created on: May 23, 2017
 *      Author: andri
 */

#ifndef MAIN_MB_TESTCODE_H_
#define MAIN_MB_TESTCODE_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_deep_sleep.h"
#include "esp_log.h"
#include "WS2812.h"

#define BLINK_GPIO GPIO_NUM_25
#define CONFIG_ENABLE_TOUCH_WAKEUP 1

#define WAKEUP_PIN_COUNT 8
#define WS2812_NUM_PIXELS 8//WAKEUP_PIN_COUNT

//#define WAKEUP_PIN_COUNT 7
//const int extWakeUpPins[8] = {27, 33, 15, 4, 2, 13, 12, 14};
//const int extWakeUpPins[WAKEUP_PIN_COUNT] = {27, 33, 15, 4, 2, 13, 14};

// #define WS2812_NUM_PIXELS WAKEUP_PIN_COUNT
//WS2812 ws2812 = WS2812(
// 	    GPIO_NUM_27, // Pin
// 		15//WAKEUP_PIN_COUNT   // Pixel count
// 	 );

extern WS2812 ws2812;

#define WAKEUP_PIN_COUNT 8
const int extWakeUpPins[WAKEUP_PIN_COUNT] = {8, 9, 3, 0}; //touch pad
static RTC_DATA_ATTR struct timeval sleep_enter_time;

void ws2812FadeInOut(uint8_t red, uint8_t green, uint8_t blue, uint8_t from, uint8_t to) {

//	float r, g, b;
//	for (int k = 0; k < 256; k = k + 1) {
//		r = (k / 256.0) * red;
//		g = (k / 256.0) * green;
//		b = (k / 256.0) * blue;
//
//		for (uint8_t pixNum = 0; pixNum < WS2812_NUM_PIXELS; pixNum++) {
//			ws2812.setPixel(pixNum, r, g, b);
//		}
//
//		ws2812.show();
//		vTaskDelay(10 / portTICK_PERIOD_MS);
//	}
//
//	for (int k = 255; k >= 0; k = k - 2) {
//		r = (k / 256.0) * red;
//		g = (k / 256.0) * green;
//		b = (k / 256.0) * blue;
//
//		for (uint8_t pixNum = 0; pixNum < WS2812_NUM_PIXELS; pixNum++) {
//			ws2812.setPixel(pixNum, r, g, b);
//		}
//
//		ws2812.show();
//		vTaskDelay(10 / portTICK_PERIOD_MS);
//	}

	for (uint8_t pixNum = 0; pixNum < WS2812_NUM_PIXELS; pixNum++) {
		ws2812.setPixel(pixNum, red, green, blue);
	}

	for (int k = from; k < (to + 1); k = k + 1) {

		for (uint8_t pixNum = 0; pixNum < WS2812_NUM_PIXELS; pixNum++) {
			ws2812.setBrightnessPercent(pixNum, (float)k);
		}

		ws2812.show();
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}

	for (int k = to; k >= from; k = k - 2) {

		for (uint8_t pixNum = 0; pixNum < WS2812_NUM_PIXELS; pixNum++) {
			ws2812.setBrightnessPercent(pixNum, (float)k);
		}

		ws2812.show();
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}

//	ws2812.show();
//	delay(2000);
//	for (uint8_t pixNum = 0; pixNum < WS2812_NUM_PIXELS; pixNum++) {
//		ws2812.setBrightnessPercent(pixNum, 10.0);
//	}
//	ws2812.show();
}

static void ws2812FadeInOutTask(void *args) {
	ws2812FadeInOut(200, 20, 80, 0, 100);
	vTaskDelete(NULL);
}

void ws2812FadingAnim(uint8_t red, uint8_t green, uint8_t blue, uint8_t from, uint8_t to) {

	for (uint8_t pixNum = 0; pixNum < WS2812_NUM_PIXELS; pixNum++) {
		ws2812.setPixel(pixNum, red, green, blue);
	}

	if (from < to) {
		for (int k = from; k < (to + 1); k = k + 1) {
			for (uint8_t pixNum = 0; pixNum < WS2812_NUM_PIXELS; pixNum++) {
				ws2812.setBrightnessPercent(pixNum, (float)k);
			}

			ws2812.show();
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
	}
	else {
		for (int k = from; k >= to; k = k - 2) {
			for (uint8_t pixNum = 0; pixNum < WS2812_NUM_PIXELS; pixNum++) {
				ws2812.setBrightnessPercent(pixNum, (float)k);
			}

			ws2812.show();
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
	}
}

static void ws2812FadingAnimTask(void *args) {
	uint8_t *from_to = (uint8_t*)args;

	ws2812FadingAnim(100, 200, 70, from_to[0], from_to[1]);
	vTaskDelete(NULL);
}

void ws2812ArrowAnim(int dir, uint8_t red, uint8_t green, uint8_t blue) {

	float r, g, b;

	if (dir == 0 || dir == 5 || dir == 6) {
		return;
	}

	dir = dir - 1;
	uint8_t ledIndiceses[4][2] = {{4, 5}, {0, 1}, {6, 7}, {2, 3}};

//	for (uint8_t pixNum = 0; pixNum < WS2812_NUM_PIXELS; pixNum++) {
//		ws2812.setPixel(pixNum, 0, 0, 0);
//	}

	uint8_t count = sizeof(ledIndiceses[dir])/sizeof(uint8_t);

//	for (int k = 0; k < 256; k = k + 1) {
//		r = (k / 256.0) * red;
//		g = (k / 256.0) * green;
//		b = (k / 256.0) * blue;
//
//		for (uint8_t idx = 0; idx < count; idx++) {
//			uint8_t pixNum = ledIndiceses[dir][idx];
//			ws2812.setPixel(pixNum, r, g, b);
//		}
//
//		ws2812.show();
//		vTaskDelay(2 / portTICK_PERIOD_MS);
//	}
//
//	for (int k = 255; k >= 0; k = k - 2) {
//		r = (k / 256.0) * red;
//		g = (k / 256.0) * green;
//		b = (k / 256.0) * blue;
//
//		for (uint8_t idx = 0; idx < count; idx++) {
//			uint8_t pixNum = ledIndiceses[dir][idx];
//			ws2812.setPixel(pixNum, r, g, b);
//		}
//
//		ws2812.show();
//		vTaskDelay(2 / portTICK_PERIOD_MS);
//	}

	for (uint8_t pixNum = 0; pixNum < WS2812_NUM_PIXELS; pixNum++) {
		ws2812.setPixel(pixNum, red, green, blue);
		ws2812.setBrightness(pixNum, 0);
	}

	for (int k = 0; k < 256; k = k + 1) {

		for (uint8_t idx = 0; idx < count; idx++) {
			uint8_t pixNum = ledIndiceses[dir][idx];
			ws2812.setBrightness(pixNum, k);
		}

		ws2812.show();
		vTaskDelay(2 / portTICK_PERIOD_MS);
	}

	for (int k = 255; k >= 0; k = k - 2) {

		for (uint8_t idx = 0; idx < count; idx++) {
			uint8_t pixNum = ledIndiceses[dir][idx];
			ws2812.setBrightness(pixNum, k);
		}

		ws2812.show();
		vTaskDelay(2 / portTICK_PERIOD_MS);
	}
}

static void ws2812ArrowAnimTask(void *args) {
	int dir = (int)args;
	ws2812ArrowAnim(dir, 100, 200, 70);
	vTaskDelete(NULL);
}

static void adc1task(void* arg)
{
    // initialize ADC
	ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_12Bit));
	ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_0db));
    while(1){
        printf("The adc1 value: %d\n",adc1_get_voltage(ADC1_CHANNEL_0));

//    	printf("The adc1 39 value: %d\n", analogRead(39));
//    	printf("The adc1 36 value: %d\n\n", analogRead(36));
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}


static void trySleep() {

	ws2812.clear();
	for (int i = 0; i < WAKEUP_PIN_COUNT; i++) {
		ws2812.setPixel(i, 0, 0, 0);
	}
	ws2812.show();

	struct timeval now;
	gettimeofday(&now, NULL);
	int sleep_time_ms = (now.tv_sec - sleep_enter_time.tv_sec) * 1000
			+ (now.tv_usec - sleep_enter_time.tv_usec) / 1000;

	switch (esp_deep_sleep_get_wakeup_cause()) {
	case ESP_DEEP_SLEEP_WAKEUP_EXT1: {
		uint64_t wakeup_pin_mask = esp_deep_sleep_get_ext1_wakeup_status();
		if (wakeup_pin_mask != 0) {
			int pin = __builtin_ffsll(wakeup_pin_mask) - 1;

//			if (pin == 15) {
//				ws2812.setPixel(1, 120, 20, 180);
//			} else if (pin == 27) {
//				ws2812.setPixel(0, 120, 20, 180);
//			} else if (pin == 12) {
//				ws2812.setPixel(3, 120, 20, 180);
//			} else if (pin == 2) {
//				ws2812.setPixel(2, 120, 20, 180);
//			}

			int n = 0;
			for(n = 0; n < WAKEUP_PIN_COUNT; n++) {
				if (extWakeUpPins[n] == pin) {
					break;
				}
			}

			if (n < WAKEUP_PIN_COUNT) {
				printf("Setting neopixel %d\n", n);
				ws2812.setPixel(n, 120, 20, 180);
			}

			printf("Wake up from GPIO %d\n", pin);
		} else {
			printf("Wake up from GPIO\n");
		}
		break;
	}
	case ESP_DEEP_SLEEP_WAKEUP_TIMER: {
		printf("Wake up from timer. Time spent in deep sleep: %dms\n",
				sleep_time_ms);

		for (int i = 0; i < WAKEUP_PIN_COUNT; i++) {
			ws2812.setPixel(i, 200, 20, 80);
		}

		break;
	}
#ifdef CONFIG_ENABLE_TOUCH_WAKEUP
	case ESP_DEEP_SLEEP_WAKEUP_TOUCHPAD: {
		touch_pad_t t = esp_deep_sleep_get_touchpad_wakeup_status();
		printf("Wake up from touch on pad %d\n", t);

		int n = 0;
		for(n = 0; n < WAKEUP_PIN_COUNT; n++) {
			if (extWakeUpPins[n] == t) {
				break;
			}
		}

		if (n < WAKEUP_PIN_COUNT) {
			printf("Setting neopixel %d\n", n);
			ws2812.setPixel(n, 20, 120, 190);
		}
		break;
	}
#endif // CONFIG_ENABLE_TOUCH_WAKEUP
#ifdef CONFIG_ENABLE_ULP_TEMPERATURE_WAKEUP
		case ESP_DEEP_SLEEP_WAKEUP_ULP: {
			printf("Wake up from ULP\n");
			int16_t diff_high = (int16_t) ulp_data_read(3);
			int16_t diff_low = (int16_t) ulp_data_read(4);
			if (diff_high < 0) {
				printf("High temperature alarm was triggered\n");
			} else if (diff_low < 0) {
				printf("Low temperature alarm was triggered\n");
			} else {
				assert(false && "temperature has stayed within limits, but got ULP wakeup\n");
			}
			break;
		}
#endif // CONFIG_ENABLE_ULP_TEMPERATURE_WAKEUP
	case ESP_DEEP_SLEEP_WAKEUP_UNDEFINED:
	default: {
		for (int i = 0; i < WAKEUP_PIN_COUNT; i++) {
			ws2812.setPixel(i, 200, 20, 80);
		}
		printf("Not a deep sleep reset\n");
	}
	}

#ifdef CONFIG_ENABLE_ULP_TEMPERATURE_WAKEUP
	if (esp_deep_sleep_get_wakeup_cause() != ESP_DEEP_SLEEP_WAKEUP_UNDEFINED) {
		printf("ULP did %d temperature measurements in %d ms\n", ulp_data_read(1), sleep_time_ms);
		printf("Initial T=%d, latest T=%d\n", ulp_data_read(0), ulp_data_read(2));
	}
#endif // CONFIG_ENABLE_ULP_TEMPERATURE_WAKEUP


	ws2812.show();

	vTaskDelay(2000 / portTICK_PERIOD_MS);

	const int wakeup_time_sec = 15;
	printf("Enabling timer wakeup, %ds\n", wakeup_time_sec);
	esp_deep_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000);

//	const int ext_wakeup_pin_1 = 15;//14;//25;
//	const uint64_t ext_wakeup_pin_1_mask = 1ULL << ext_wakeup_pin_1;
//	const int ext_wakeup_pin_2 = 27;//12;//26;
//	const uint64_t ext_wakeup_pin_2_mask = 1ULL << ext_wakeup_pin_2;
//	const int ext_wakeup_pin_3 = 12;//12;//26;
//	const uint64_t ext_wakeup_pin_3_mask = 1ULL << ext_wakeup_pin_3;
//	const int ext_wakeup_pin_4 = 2;//12;//26;
//	const uint64_t ext_wakeup_pin_4_mask = 1ULL << ext_wakeup_pin_4;
//
//	printf("Enabling EXT1 wakeup on pins GPIO%d, GPIO%d, GPIO%d, GPIO%d\n", ext_wakeup_pin_1, ext_wakeup_pin_2, ext_wakeup_pin_3, ext_wakeup_pin_4);
//	esp_deep_sleep_enable_ext1_wakeup(ext_wakeup_pin_1_mask | ext_wakeup_pin_2_mask | ext_wakeup_pin_3_mask | ext_wakeup_pin_4_mask, ESP_EXT1_WAKEUP_ANY_HIGH);

//	uint64_t extWakeUpPinMasks[8];
//	for(int j = 0; j < WAKEUP_PIN_COUNT; j++) {
//		extWakeUpPinMasks[j] = 1ULL << extWakeUpPins[j];
//	}
//
//	uint64_t extWakeUpMask = 0;
//	for(int j = 0; j < WAKEUP_PIN_COUNT; j++) {
//		extWakeUpMask |= extWakeUpPinMasks[j];
//	}
//	printf("Enabling EXT1 wakeup on few pins\n");
//	esp_deep_sleep_enable_ext1_wakeup(extWakeUpMask, ESP_EXT1_WAKEUP_ANY_HIGH);


//	CapacitiveTouch::TouchActionCallback touchCb = [=](uint8_t gpio) {
//			ESP_LOGI(TAG, "Touch %d is touched! %d", gpio, touchRead(gpio));
//	//		String payload = "{\"touch\":" + String(gpio) + "}";
//	//		mqtt->publish(topicData, payload.c_str());
//	};
//
//	MagicButton.activateTouch(27, touchCb);

//	esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_AUTO);

	touch_pad_init();
	calibrate_touch_pad(TOUCH_PAD_NUM8);
	calibrate_touch_pad(TOUCH_PAD_NUM9);
//	//touch_pad_config(TOUCH_PAD_NUM5, 50);
////	touch_pad_config(TOUCH_PAD_NUM6, 50);
	calibrate_touch_pad(TOUCH_PAD_NUM3);
	calibrate_touch_pad(TOUCH_PAD_NUM0);

////	touch_pad_config(TOUCH_PAD_NUM7, 200);
	printf("Enabling touch pad wakeup\n");
	ESP_ERROR_CHECK(esp_deep_sleep_enable_touchpad_wakeup());
//
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	printf("Entering deep sleep\n");
	gettimeofday(&sleep_enter_time, NULL);

	#ifdef CONFIG_ENABLE_ULP_TEMPERATURE_WAKEUP
		start_ulp_temperature_monitoring();
	#endif

	for (int i = 0; i < WAKEUP_PIN_COUNT; i++) {
		ws2812.setPixel(i, 0, 0, 0);
	}

	ws2812.show();

	vTaskDelay(500 / portTICK_PERIOD_MS);

	esp_deep_sleep_start();
}

/*
#include <Wire.h>
#include <SparkFun_APDS9960.h>

#define APDS9960_INT 	5
// Constants
#define PROX_INT_HIGH   50 // Proximity level for interrupt
#define PROX_INT_LOW    0  // No far interrupt

SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint8_t proximity_data = 0;
volatile bool isr_flag = 0;

void interruptRoutine() {
	isr_flag = 1;
}

void handleGesture() {
	if (apds.isGestureAvailable()) {
		switch (apds.readGesture()) {
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
	}
}

static void tryGesture(void* arg) {
	pinMode(APDS9960_INT, INPUT_PULLUP);

	// Initialize Serial port
	Serial.println();
	Serial.println(F("---------------------------------------"));
	Serial.println(F("SparkFun APDS-9960 - ProximityInterrupt"));
	Serial.println(F("---------------------------------------"));

	// Initialize APDS-9960 (configure I2C and initial values)
	if (apds.init()) {
		Serial.println(F("APDS-9960 initialization complete"));
	} else {
		Serial.println(F("Something went wrong during APDS-9960 init!"));
	}

	// Adjust the Proximity sensor gain
	if (!apds.setProximityGain(PGAIN_2X)) {
		Serial.println(F("Something went wrong trying to set PGAIN"));
	}

	// Set proximity interrupt thresholds
	if (!apds.setProximityIntLowThreshold(PROX_INT_LOW)) {
		Serial.println(F("Error writing low threshold"));
	}
	if (!apds.setProximityIntHighThreshold(PROX_INT_HIGH)) {
		Serial.println(F("Error writing high threshold"));
	}

	// Initialize interrupt service routine
	attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);

	// Start running the APDS-9960 proximity sensor (interrupts)
	if (apds.enableProximitySensor(true)) {
		Serial.println(F("Proximity sensor is now running"));
	} else {
		Serial.println(F("Something went wrong during sensor init!"));
	}

//	if (!apds.setGestureLEDDrive(2)) {
//		Serial.println(F("Something went wrong trying to set Gesture LED drive"));
//	}

	// Start running the APDS-9960 gesture sensor engine
	if (apds.enableGestureSensor(true)) {
		Serial.println(F("Gesture sensor is now running"));
	} else {
		Serial.println(F("Something went wrong during gesture sensor init!"));
	}

	for(;;) {

		// If interrupt occurs, print out the proximity level
		if (isr_flag == 1) {
//		if(digitalRead(APDS9960_INT) == 0){


			// Read proximity level and print it out
			if (!apds.readProximity(proximity_data)) {
				Serial.println("Error reading proximity value");
			} else {
				Serial.print("Proximity detected! Level: ");
				Serial.println(proximity_data);
			}

			// Reset flag and clear APDS-9960 interrupt (IMPORTANT!)
			isr_flag = 0;
			if (!apds.clearProximityInt()) {
				Serial.println("Error clearing interrupt");
			}


//			Serial.println("Gesture");

//			detachInterrupt(APDS9960_INT);
			handleGesture();
//			if(digitalRead(APDS9960_INT) == 0){
//			  apds.init();
//			  apds.enableGestureSensor(true);
//			}
//			delay(100);
//			isr_flag = 0;
//			attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);
		}


		delay(1);
	}
}
*/

#include <stdio.h>
#include "driver/i2s.h"
#include <math.h>

#define SAMPLE_RATE     (36000)
#define I2S_NUM         (0)
#define WAVE_FREQ_HZ    (100)

#define SAMPLE_PER_CYCLE (SAMPLE_RATE/WAVE_FREQ_HZ)

void tryI2S(void)
{
    unsigned int i, sample_val;
    float sin_float, triangle_float, triangle_step = 65536.0 / SAMPLE_PER_CYCLE;
    //for 36Khz sample rates, we create 100Hz sine wave, every cycle need 36000/100 = 360 samples (4-bytes each sample)
    //using 6 buffers, we need 60-samples per buffer
    //2-channels, 16-bit each channel, total buffer is 360*4 = 1440 bytes
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),                                  // Only TX
        .sample_rate = SAMPLE_RATE,
        //.bits_per_sample = I2S_BITS_PER_SAMPLE_8BIT,                                                  //16-bit per channel
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,                                                  //16-bit per channel
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           //2-channels
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),                                                      //
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,                                //Interrupt level 1
        .dma_buf_count = 8,
        .dma_buf_len = 64
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = 26,
        .ws_io_num = 25,
        .data_out_num = 22,
        .data_in_num = -1                                                       //Not used
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
//    i2s_set_pin(I2S_NUM_0, NULL);

    triangle_float = -32767;

    for(i = 0; i < SAMPLE_PER_CYCLE; i++) {
        sin_float = sin(i * PI / 180.0);
        if(sin_float >= 0)
            triangle_float += triangle_step;
        else
            triangle_float -= triangle_step;
        sin_float *= 32767;

        sample_val = 0;
        sample_val += (short)triangle_float;
        sample_val = sample_val << 16;
        sample_val += (short) sin_float;

        i2s_push_sample(I2S_NUM_0, (char *)&sample_val, portMAX_DELAY);
    }
}

#include "FilesystemHandler.h"
#include "SpiffsFile.h"
#include "spiffs.h"
#include <dirent.h>
#include <sys/stat.h>

//#define REC_BUFF_SIZE 80*1024
//uint8_t *playbackBuffer = (uint8_t *)malloc(REC_BUFF_SIZE * sizeof(uint8_t));

//SpiffsFile *recordFile;

// void tryI2SInput(void *p)
// {
// 	i2s_config_t i2s_config_rx = {
// 		mode: (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
// 		sample_rate: 44100,
// 		bits_per_sample: I2S_BITS_PER_SAMPLE_32BIT,
// 		channel_format: I2S_CHANNEL_FMT_RIGHT_LEFT,
// 		communication_format: (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
// 		intr_alloc_flags: ESP_INTR_FLAG_LEVEL1,
// 		dma_buf_count: 14,
// 		dma_buf_len: 64
// 	};

// 	i2s_config_t i2s_config_tx = {
// 		mode: (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
// 		sample_rate: 44100,
// 		bits_per_sample: I2S_BITS_PER_SAMPLE_32BIT,
// 		channel_format: I2S_CHANNEL_FMT_RIGHT_LEFT,
// 		communication_format: (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
// 		intr_alloc_flags: ESP_INTR_FLAG_LEVEL1,
// 		dma_buf_count: 32,
// 		dma_buf_len: 64
// 	};

//     i2s_pin_config_t pin_config = {
//         .bck_io_num = 26, //21,
//         .ws_io_num = 25,
//         .data_out_num = 22,//I2S_PIN_NO_CHANGE
//         .data_in_num = 39
//     };

// 	//Somehow it's needed. If not, noise!
//  	// pinMode(23, INPUT);
// 	pinMode(39, INPUT);
// 	// pinMode(22, OUTPUT);

// 	//for recording
//  	i2s_driver_install(I2S_NUM_1, &i2s_config_rx, 0, NULL);
//  	i2s_set_pin(I2S_NUM_1, &pin_config);

//  	//for playback
//  	i2s_driver_install(I2S_NUM_0, &i2s_config_tx, 0, NULL);
//  	i2s_set_pin(I2S_NUM_0, &pin_config);


// //	int cnt = 0;
// //	uint64_t buffer = 0;
// //	char buf[2048];

// //	while (1) {
// //		cnt++;
// //		buffer = 0;
// //
// //		int bytes_read = i2s_read_bytes(I2S_NUM_1, buf, sizeof(buf), portMAX_DELAY);
// //
// //		int samples_read = bytes_read / (I2S_BITS_PER_SAMPLE_32BIT / 8);
// //
// //		char *buf_ptr_read = &buf[0];
// //		char *buf_ptr_write = &buf[0];
// //		//const char samp64[8] = {};
// //		for (int i = 0; i < samples_read; i++) {
// //			buf_ptr_write[0] = buf_ptr_read[2]; // mid
// //			buf_ptr_write[1] = buf_ptr_read[3]; // high
// //
// //			buf_ptr_write += 1 * (I2S_BITS_PER_SAMPLE_32BIT / 8);
// //			buf_ptr_read += 2 * (I2S_BITS_PER_SAMPLE_32BIT / 8);
// //
// ////			Serial.printf("%d -> %x\n", cnt, buf[i]);
// //		}
// //
// //		int readable_bytes = samples_read * (I2S_BITS_PER_SAMPLE_32BIT / 8);
// //
// //		// 2 * mono
// //		i2s_write_bytes(I2S_NUM_0, (const char*)buf, readable_bytes, portMAX_DELAY);
// //		i2s_write_bytes(I2S_NUM_0, (const char*)buf, readable_bytes, portMAX_DELAY);
// //
// //		delay(1);
// //	}

// 	uint32_t cnt = 0;
// 	uint32_t buffer;
// 	uint32_t buffer_out = 0;
// 	while (1) {
// 		cnt++;
// 		buffer = 0;
// 		int bytes_popped = i2s_pop_sample(I2S_NUM_1, (char*) &buffer,
// 				portMAX_DELAY);

// 		buffer_out = buffer << 5;

// 		if (buffer_out == 0) {
// 			//For debugging, if out is zero
// 			Serial.printf("%d -> %x\n", cnt, (int) buffer_out);
// 			delay(50);
// 		} else {
// 			//Just playback for now
// 			i2s_push_sample(I2S_NUM_0, (char*) &buffer_out, portMAX_DELAY);
// 		}
// 	}

//    	vTaskDelete(NULL);
// }

bool do_fs_test()
{
    FilesystemHandler *fs_handler = FilesystemHandler::get_instance();

	#define CHUNK 150
    char buf[CHUNK];
    int nread = -1;

    //Read using spiffs api
    printf("Trying to read using SPIFFS API!\n");
    spiffs_file f = SPIFFS_open(&fs_handler->fs, "/musics/happy.wav", SPIFFS_O_RDONLY, 0);

    printf("Size: %d\n", (size_t)f);
    if (f) {
//        while ((nread = SPIFFS_read(&fs_handler->fs, f, buf, sizeof buf)) > 0)
//        {
//            printf("%.*s", nread, buf);
//        }
        printf("%d\n", nread);
        SPIFFS_close(&fs_handler->fs, f);
        printf("\n");
    }
    printf("Now trying to read using VFS API!\n");

    FILE *file;
    file = fopen("/spiffs/musics/happy.wav", "rw");
    printf("%d\n", (size_t)f);

    if (file) {
//        while (fgets(buf, 50, file) != nullptr)
//        {
//            printf("%s", buf);
//        }

    	fseek(file, 0, SEEK_END); // seek to end of file
    	long size = ftell(file); // get current file pointer
    	fseek(file, 0, SEEK_SET); // seek back to beginning of file
        fclose(file);

        printf("size: %d\n", (size_t)f);
        printf("\n");

    }
    else return false;
    printf("Now trying to list the files in the spiffs partition!\n");
    printf("We first try using SPIFFS calls directly.\n");

    spiffs_DIR dir;
	struct spiffs_dirent dirEnt;
	const char rootPath[] = "/musics/";

	if (SPIFFS_opendir(&fs_handler->fs, rootPath, &dir) == NULL) {
		printf("Unable to open %s dir", rootPath);
		return false;
	}
	while(SPIFFS_readdir(&dir, &dirEnt) != NULL) {
		int len = strlen((char *)dirEnt.name);
		// Skip files that end with "/."
		if (len>=2 && strcmp((char *)(dirEnt.name + len -2), "/.") == 0) {
			continue;
		}
        printf("%s\n", dirEnt.name);
    }
    SPIFFS_closedir(&dir);

    printf("We now try through the filesystem.\n");
    DIR *midir;
    struct dirent* info_archivo;
    if ((midir = opendir("/spiffs/musics/")) == NULL)
    {
        perror("Error in opendir\n");
    }
    else
    {
        while ((info_archivo = readdir(midir)) != 0)
            printf ("%s \n", info_archivo->d_name);
    }
    closedir(midir);

    printf("We see if stat is working by accessing a file which we know is there.\n");
    struct stat stat_buf;
    printf("Stat result:%d\n", stat("/spiffs/musics/happy.wav", &stat_buf));

    printf("Stat result:%d\n", stat("/spiffs/musics/.", &stat_buf));

    return true;
}

//
//#include "wavspiffs.h"
////#include "WAVFile.h"
//
//void readWav() {
//
//	wavFILE_t wf;
//	wavProperties_t wProps;
//	int rc = wavOpen("/musics/happy.wav", &wf, &wProps);
//
//	//wavProperties_t wProps;
////	WAVFile f;
////	int rc;
//
////	if (f.Open("/musics/T2.wav")) {
////
////		Serial.printf("audioFormat %d\r\n", f.properties.audioFormat);
////		Serial.printf("numChannels %d\r\n", f.properties.numChannels);
////		Serial.printf("sampleRate %d\r\n", f.properties.sampleRate);
////		Serial.printf("byteRate %d\r\n", f.properties.byteRate);
////		Serial.printf("blockAlign %d\r\n", f.properties.blockAlign);
////		Serial.printf("bitsPerSample %d\r\n", f.properties.bitsPerSample);
////	}
//
//	int sampleRate = wProps.sampleRate;//(int)f.properties.sampleRate
//	i2s_config_t i2s_config = {
//	        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),                                  // Only TX
//	        .sample_rate = sampleRate,
//	        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,                                                  //16-bit per channel
//	        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           //2-channels
//	        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S),                                                      //
//	        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,                                //Interrupt level 1
//	        .dma_buf_count = 16,
//	        .dma_buf_len = 128
//	};
//
//	i2s_pin_config_t pin_config = {
//		.bck_io_num = 26,
//		.ws_io_num = 25,
//		.data_out_num = 22,
//		.data_in_num = -1                                                       //Not used
//	};
//
//	i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
//	i2s_set_pin(I2S_NUM_0, &pin_config);
//	//i2s_set_pin(I2S_NUM_0, NULL);
//
//	i2s_start(I2S_NUM_0);
//
//	bool i2s_full = false;
//
//	int16_t buffer[512];
//	int bufferlen = -1;
//	int buffer_index = 0;
//	int playing = true;
//
//	unsigned char attenuation = (100 - 80) / 16;
//	unsigned int sample_val;
//
//	while (playing && !i2s_full) {
//		while (buffer_index < bufferlen) {
//			int16_t pcm = buffer[buffer_index];
//
////			int16_t left = pcm;
////			int16_t right = pcm;
////
////			left = (left >> 8) + 0x80;
////			right = (right >> 8) + 0x80;
////			unsigned short sample = (left << 8) | (right & 0xff);
//
////			int sample = right & 0xFFFF;
////			sample = sample << 16;
////			sample |= left & 0xFFFF;
//
//			sample_val = 0;
//			sample_val += (short) pcm >> attenuation;
//			sample_val = sample_val << 16;
//			sample_val += (short) pcm >> attenuation;
//
//			int ret = i2s_push_sample(I2S_NUM_0, (char *)&sample_val, portMAX_DELAY);
//			//Serial.printf("wavRead %d\r\n", ret);
//			//if (i2s_write_lr_nb(pcm, pcm)) {
//			if (ret) {
//				buffer_index++;
//			} else {
//				i2s_full = true;
//				break;
//			}
//			if ((buffer_index & 0x3F) == 0)
//				delay(1);
//		}
//
//		if (i2s_full)
//			break;
//
//		rc = wavRead(&wf, buffer, sizeof(buffer));
//		//rc = f.Read((uint8_t*)buffer, sizeof(buffer));
//
//		if (rc > 0) {
//			Serial.printf("wavRead %d\r\n", rc);
//			bufferlen = rc / sizeof(buffer[0]);
//			buffer_index = 0;
//
//			delay(1);
//
//		} else {
//			Serial.println(F("Stop playing"));
//
//			i2s_stop(I2S_NUM_0);
//			playing = false;
//			wavClose(&wf);
//			//f.Close();
//
//			break;
//		}
//	}
//}

#endif /* MAIN_MB_TESTCODE_H_ */
