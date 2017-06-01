/*
 * MusicPlayer.cpp
 *
 *  Created on: 10.04.2017
 *      Author: bernd
 */

#include "I2SPlayer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <driver/gpio.h>
#include <driver/i2s.h>
#include <esp_log.h>
#include <esp_err.h>


#define I2S_NUM I2S_NUM_0

I2SPlayer::I2SPlayer() {
	// TODO Auto-generated constructor stub

}

I2SPlayer::~I2SPlayer() {
	if (wavFile != nullptr) {
		wavFile = nullptr;
		delete wavFile;
	}
}

void I2SPlayer::init() {

	i2s_config_t i2s_config;

	i2s_config.mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX); // Only TX
	i2s_config.sample_rate = 16 * 1024; // 16kHz -- override later
	i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT; //16-bit per channel
	i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;         //2-channels
	i2s_config.communication_format = I2S_COMM_FORMAT_I2S; //| I2S_COMM_FORMAT_I2S_MSB,
	i2s_config.dma_buf_count = 16;
	i2s_config.dma_buf_len = 128;                          //
	i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;      //Interrupt level 1

	i2s_pin_config_t pin_config;
	pin_config.bck_io_num = GPIO_NUM_21; // BCL Bus Clock
	pin_config.ws_io_num = GPIO_NUM_26; // LRC
	pin_config.data_out_num = GPIO_NUM_22; // DataIn on Amp
	pin_config.data_in_num = I2S_PIN_NO_CHANGE;//-1;                                      //Not used

	i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
	i2s_set_pin(I2S_NUM, &pin_config);

//	i2s_config.mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN); // Only TX
//	i2s_config.sample_rate = 16 * 1024; // 16kHz -- override later
//	i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_8BIT; //16-bit per channel
//	i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;         //2-channels
//	i2s_config.communication_format = I2S_COMM_FORMAT_I2S_MSB,
//	i2s_config.dma_buf_count = 8;
//	i2s_config.dma_buf_len = 64;                          //
//	i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;      //Interrupt level 1
//
//	i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
//	i2s_set_pin(I2S_NUM, NULL);

}

bool I2SPlayer::prepareWav(const char* wavFilename) {
	wavFile = new WAVFile();

	if (wavFile->Open(wavFilename)) {

		printf("audioFormat %d\r\n", wavFile->properties.audioFormat);
		printf("numChannels %d\r\n", wavFile->properties.numChannels);
		printf("sampleRate %d\r\n", wavFile->properties.sampleRate);
		printf("byteRate %d\r\n", wavFile->properties.byteRate);
		printf("blockAlign %d\r\n", wavFile->properties.blockAlign);
		printf("bitsPerSample %d\r\n", wavFile->properties.bitsPerSample);

		ESP_ERROR_CHECK(i2s_set_sample_rates(I2S_NUM, wavFile->properties.sampleRate));
		return true;
	}
	else {
		delete wavFile;
		wavFile = nullptr;
	}

	return false;
}

bool I2SPlayer::play() {
	if (!wavFile->IsInitialized()) {
		return false;
	}

	unsigned char attenuation = (100 - volume) / 16;

	//wavFile->Rewind(); //causing click sound

	i2s_start(I2S_NUM);

	unsigned int sample_val;
	int sample = 0;
	while (wavFile->NextSample(&sample)) {

		sample_val = 0;
		sample_val += (short) sample >> attenuation;
		sample_val = sample_val << 16;
		sample_val += (short) sample >> attenuation;

		i2s_push_sample(I2S_NUM, (char*) &sample_val, portMAX_DELAY);
	}

	i2s_stop(I2S_NUM);
	wavFile->Close();

	return true;
}

void task_function_play(void *pvParameter)
{
	((I2SPlayer*)pvParameter)->play();
	vTaskDelete(NULL);
}

void I2SPlayer::playAsync() {
	TaskHandle_t xHandle = NULL;
	xTaskCreate(&task_function_play, "I2SPlay", 4096, this, 5, &xHandle);
	//xTaskCreate(&task_function_play, "I2SPlay", 4096, this, 5, NULL);
}

void I2SPlayer::setVolume(unsigned char volume) {
	if (volume > 100) {
		volume = 100;
	}
	this->volume = volume;
}
