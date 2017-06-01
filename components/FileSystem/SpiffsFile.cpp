/*
 * SpiffsFileHandle.cpp
 *
 *  Created on: 11.04.2017
 *      Author: bernd
 */

#include "SpiffsFile.h"

#include <esp_partition.h>
#include <esp_spiffs.h>
#include <esp_log.h>
#include <esp_err.h>

SpiffsFile::SpiffsFile() {
//	fs_handler = FilesystemHandler::get_instance((char *)"spiffs"/*Mount point */);
//	fs_handler->init_spiffs();
	fs_handler = FilesystemHandler::get_instance();
}


SpiffsFile::~SpiffsFile() {
	Close();
}

bool SpiffsFile::Open(std::string s, spiffs_flags openFlags) {
	//fileHandle = SPIFFS_open(&fs_handler->fs, s.c_str(), SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
	fileHandle = SPIFFS_open(&fs_handler->fs, s.c_str(), openFlags, 0);
	//printf("Open status %d\n", fileHandle);
	printf("Open status %i\n", SPIFFS_errno(&fs_handler->fs));
	return fileHandle == 1;
}

int SpiffsFile::Write(uint8_t* data, int size) {
	int written = SPIFFS_write(&fs_handler->fs, fileHandle, data, size);
	if (written > 0) {
		bytesWritten += written;
	}
	return written;
}

int SpiffsFile::Read(uint8_t* buf, int maxlen) {
	return SPIFFS_read(&fs_handler->fs, fileHandle, buf, maxlen);
}

void SpiffsFile::Close() {
	SPIFFS_close(&fs_handler->fs, fileHandle);
}

std::vector<char> SpiffsFile::Read(int maxread) {
	std::vector<char> buf;
	buf.reserve(maxread);
	buf.resize(SPIFFS_read(&fs_handler->fs, fileHandle, (void*)buf.data(), maxread));
	return buf;
}

int SpiffsFile::Write(std::vector<char> vector) {
	return SPIFFS_write(&fs_handler->fs, fileHandle, (void*)vector.data(), vector.size());
}

int SpiffsFile::FileStat(spiffs_stat* stat) {
	return SPIFFS_fstat(&fs_handler->fs, fileHandle, stat);
}

int32_t SpiffsFile::Seek(s32_t offs, int whence) {
	return SPIFFS_lseek(&fs_handler->fs, fileHandle, offs, whence);
}

int SpiffsFile::Rewind() {
	return SPIFFS_lseek(&fs_handler->fs, fileHandle, 0, SPIFFS_SEEK_SET);
}
