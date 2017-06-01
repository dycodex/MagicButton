/*
 * MB_TestCap.h
 *
 *  Created on: May 28, 2017
 *      Author: andri
 */

#ifndef MAIN_MB_TESTCAP_H_
#define MAIN_MB_TESTCAP_H_

#include "driver/touch_pad.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"

static void calibrate_touch_pad(touch_pad_t pad)
{
    touch_pad_config(pad, 1000);

    int avg = 0;
    const size_t calibration_count = 64;
    for (int i = 0; i < calibration_count; ++i) {
        uint16_t val;
        touch_pad_read(pad, &val);
        avg += val;
    }
    avg /= calibration_count;
    const int min_reading = 300;
    if (avg < min_reading) {
        printf("Touch pad #%d average reading is too low: %d (expecting at least %d). "
               "Not using for deep sleep wakeup.\n", pad, avg, min_reading);
        touch_pad_config(pad, 0);
    } else {
        int threshold = avg - 100;
        printf("Touch pad #%d average: %d, wakeup threshold set to %d.\n", pad, avg, threshold);
        touch_pad_config(pad, threshold);
    }
}

static void touchpad_read_task(void *pvParameter)
{
    while (1) {
        uint16_t touch_value;
        for (int i=0; i<TOUCH_PAD_MAX; i++) {
        	if (i == 7 || i == 1) {
        		continue;
        	}
            ESP_ERROR_CHECK(touch_pad_read((touch_pad_t)i, &touch_value));
            printf("T%d:%4d ", i, touch_value);
        }
        printf("\n");
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}

static void touchpad_circular_task(void *pvParameter)
{
	uint8_t wheelPads = 8;

	int16_t wheelBaseline[wheelPads];
	int i = 0;
	for (int pad = 0; pad <TOUCH_PAD_MAX; pad++) {
		if (pad == 7 || pad == 1) {
			continue;
		}

		int avgVal = 0;
		const size_t calibration_count = 8;
		for (int i = 0; i < calibration_count; ++i) {
			uint16_t touch_value;
			touch_pad_read((touch_pad_t)pad, &touch_value);
			avgVal += touch_value;
		}
		avgVal /= calibration_count;

		wheelBaseline[i] = avgVal;
		i++;
	}

	while (1) {

		int16_t wheelDeltas[wheelPads];

		i = 0;

		for (int pad = 0; pad <TOUCH_PAD_MAX; pad++) {
			if (pad == 7 || pad == 1) {
				continue;
			}

			int avgVal = 0;
			const size_t calibration_count = 4;
			for (int i = 0; i < calibration_count; ++i) {
				uint16_t touch_value;
				touch_pad_read((touch_pad_t)pad, &touch_value);
				avgVal += touch_value;
			}
			avgVal /= calibration_count;

			int diff = wheelBaseline[i] - avgVal;
			wheelDeltas[i] = diff;

			i++;
		}

		int16_t maxVal = 0;
		int16_t maxIdx = -1;
		//Serial.print("Delta = ");
		for (i = 0; i < wheelPads; i++) {
			//Serial.printf("%d, ", wheelDeltas[i]);

			if (wheelDeltas[i] > maxVal) {
				maxVal = wheelDeltas[i];
				maxIdx = i;
		    }
		}

		//Serial.printf("\nMax = %d %d\n", maxIdx, maxVal);

		if (maxVal < 70) {    // was <20
//			Serial.println("FCUK");
		    continue;
		}


		// Find neighbouring values
		int16_t v1Idx = (maxIdx-1);
		if (v1Idx < 0) v1Idx += wheelPads; // Handle the negitive modulo

		int16_t v1 = wheelDeltas[v1Idx];                   // Pad before
		int16_t v2 = wheelDeltas[maxIdx];                  // Pad with highest value
		int16_t v3 = wheelDeltas[(maxIdx+1) % wheelPads];  // Pad after
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
			offsetPad += wheelPads; // Bound check incase we go negitive for half of the first wheel

		// Calculate the touch position (in degrees)
		int16_t degree = floor((offsetPad / wheelPads) * 360.0f) - -20; //-20 = offset
		if (degree < 0)
			degree += 360;
		if (degree > 360)
			degree -= 360;


		Serial.printf("Degree = %d\n", degree);

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}


#endif /* MAIN_MB_TESTCAP_H_ */
