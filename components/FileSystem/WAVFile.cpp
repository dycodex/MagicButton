/*
 * WAVFile.cpp
 *
 *  Created on: Apr 30, 2017
 *      Author: andri
 */

#include "WAVFile.h"

#define CCCC(c1, c2, c3, c4)    ((c4 << 24) | (c3 << 16) | (c2 << 8) | c1)

WAVFile::WAVFile():SpiffsFile() {
}

WAVFile::~WAVFile() {
}

int WAVFile::readuint32(uint32_t *ui32)
{
    int rc;

    //rc = wf->f.read((uint8_t *)ui32, sizeof(*ui32));
    //rc = fread((uint8_t *)ui32, sizeof(uint32_t), 1, wf->f);

    //rc = SPIFFS_read(&fs_handler->fs, wf->f, (uint8_t *)ui32, sizeof(uint32_t));
    rc = this->Read((uint8_t *)ui32, (int)sizeof(uint32_t));

    //Serial.printf("readuint32 rc=%d val=0x%X\r\n", rc, *ui32);
    return rc;
}

bool WAVFile::Open(std::string s) {
	return this->Open(s, &properties);
}

bool WAVFile::Open(std::string s, wavProperties_t* wavProps) {

	typedef enum headerState_e {
		HEADER_INIT, HEADER_RIFF, HEADER_FMT, HEADER_LIST, HEADER_DATA
	} headerState_t;

	headerState_t state = HEADER_INIT;
	uint32_t chunkID, chunkSize;
	int retval;

	if (!SpiffsFile::Open(s, SPIFFS_O_RDONLY)) {
		retval = -1;
		goto closeExit;
	}

	printf("WAV file is opened OK\n");

	while (state != HEADER_DATA) {
		if (readuint32(&chunkID) != 4) {
			retval = -2;
			goto closeExit;
		}
		if (readuint32(&chunkSize) != 4) {
			retval = -3;
			goto closeExit;
		}
		switch (chunkID) {
		case CCCC('R', 'I', 'F', 'F'):
			if (readuint32(&chunkID) != 4) {
				retval = -4;
				goto closeExit;
			}
			if (chunkID != CCCC('W', 'A', 'V', 'E')) {
				retval = -5;
				goto closeExit;
			}
			state = HEADER_RIFF;
			printf("RIFF %d\r\n", chunkSize);
			break;

		case CCCC('f', 'm', 't', ' '):
			if (this->Read((uint8_t *) wavProps, sizeof(*wavProps)) != sizeof(*wavProps)) {
				retval = -6;
				goto closeExit;
			}
			state = HEADER_FMT;
			printf("fmt  %d\r\n", chunkSize);
			if (chunkSize > sizeof(*wavProps)) {
				//SPIFFS_lseek(&fs_handler->fs, wf->f, chunkSize - sizeof(*wavProps), SPIFFS_SEEK_CUR);
				this->Seek(chunkSize - sizeof(*wavProps), SPIFFS_SEEK_CUR);
			}
			break;

		case CCCC('d', 'a', 't', 'a'):
			state = HEADER_DATA;
			printf("data %d\r\n", chunkSize);
			break;

		case CCCC('L', 'I', 'S', 'T'):
			state = HEADER_LIST;
			printf("LIST %d\r\n", chunkSize);
			if (!this->Seek(chunkSize, SPIFFS_SEEK_CUR)) {
				retval = -7;
				goto closeExit;
			}
			break;

		default:
			printf("%08X %d\r\n", chunkID, chunkSize);
			//if (!wf->f.seek(chunkSize, SeekCur)) {
			if (this->Seek(chunkSize, SPIFFS_SEEK_CUR)) {
				retval = -7;
				goto closeExit;
			}
		}
	}

	if (state == HEADER_DATA) {
//		retval = 0;
		initialized = true;
		return true;
	}
	else {
		retval = -8;
	}

	closeExit: this->Close();

	printf("Return value is %d\n", retval);

	return (retval == 0);
}

bool WAVFile::NextSample(int* sample) {

	if (!initialized) {
		return false;
	}

	if (buffer_index >= bufferlen) {

		//read to buffer
		int rc = this->Read((uint8_t*)buffer, sizeof(buffer));

		if (rc > 0) {
			//printf("wavRead %d\r\n", rc);
			bufferlen = rc / sizeof(buffer[0]);
			buffer_index = 0;

		} else {
			printf("Stop playing\n");

			//playing = false;
			this->Close();
			return false;
		}
	}

//		int16_t pcm = buffer[buffer_index];
//		buffer_index++;

	if (properties.bitsPerSample == 8) {
		*sample = (uint8_t)buffer[buffer_index];
		*sample -= 127;
		*sample = *sample << 8;
		buffer_index += sizeof(uint8_t);

	} else if (properties.bitsPerSample == 16 || properties.bitsPerSample == 0) {

		*sample = (int16_t)((buffer[buffer_index+1] << 8) | buffer[buffer_index]);
		buffer_index += sizeof(int16_t);

//			*sample = (int16_t)buffer[buffer_index];
//			buffer_index++;

	} else {
		printf("Bit-depth not supported: %u\n", properties.bitsPerSample);
		return false;
	}

	return true;
}

int WAVFile::Rewind() {
	bufferlen = -1;
	buffer_index = 0;

	return SpiffsFile::Rewind();
}

