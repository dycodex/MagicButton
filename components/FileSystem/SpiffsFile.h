/*
 * SpiffsFileHandle.h
 *
 *  Created on: 11.04.2017
 *      Author: bernd
 */

#ifndef MAIN_SPIFFSFILE_H_
#define MAIN_SPIFFSFILE_H_

#include <stdlib.h>
#include <string>
#include <vector>
#include "spiffs.h"

#include "FilesystemHandler.h"

class FilesystemHandler;

/*
 * Methods are uppercase due to clash with macros "read", "write", ...
 */
class SpiffsFile {
public:
	SpiffsFile();
	virtual ~SpiffsFile();

	bool Open(std::string s, spiffs_flags openFlags = SPIFFS_O_RDONLY);
	int Write(uint8_t* data, int size);
	int Read(uint8_t* buf, int maxlen);
	int Write(std::vector<char>);
	int Rewind();
	std::vector<char> Read(int maxread);
	void Close();
	int FileStat(spiffs_stat *stat);
	int32_t Seek(s32_t offs, int whence);

	int getBytesWritten() { return bytesWritten; }

protected:
	spiffs_file fileHandle = 0;
	size_t bytesWritten = 0;
	FilesystemHandler *fs_handler;
};

#endif /* MAIN_SPIFFSFILE_H_ */
