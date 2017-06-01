/*
 * WAVFile.h
 *
 *  Created on: Apr 30, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_FILESYSTEM_WAVFILE_H_
#define COMPONENTS_FILESYSTEM_WAVFILE_H_

#include "SpiffsFile.h"

typedef struct wavProperties_s {
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
} wavProperties_t;

class WAVFile: public SpiffsFile {
public:
	WAVFile();
	virtual ~WAVFile();

	bool Open(std::string s, wavProperties_t *wavProps);
	bool Open(std::string s);
	int Rewind();
	bool IsInitialized() { return initialized; }

	bool NextSample(int *sample);

	wavProperties_t properties;


private:
	int readuint32(uint32_t *ui32);

	bool initialized = false;
	int bufferlen = -1;
	int buffer_index = 0;
	uint8_t buffer[512];
};

#endif /* COMPONENTS_FILESYSTEM_WAVFILE_H_ */
