#pragma once
#define LOG_PAGE_SIZE 256
extern "C"
{
    #include "spiffs.h"
}
#include <cstdint>
#include "SpiffsFile.h"

class FilesystemHandler;

class FilesystemHandler
{
	friend class SpiffsFile;
    public:
//        static FilesystemHandler *get_instance() { return FilesystemHandler::instance; };
//        static FilesystemHandler *get_instance(size_t _addr, size_t _size, char * _mountpt);
//        size_t addr;
//        size_t size;

		static FilesystemHandler *get_instance() {
//			if(FilesystemHandler::instance == nullptr)
//			{
//				printf("NULLLLLL\n");
//			}
			return FilesystemHandler::instance;
		};
		static FilesystemHandler *get_instance(char * _mountpt);
        char * mountpt;
        bool init_spiffs();
        void test();
        spiffs fs;
    private:
        static FilesystemHandler *instance;
        uint8_t spiffs_work_buf[LOG_PAGE_SIZE*4];
        uint8_t spiffs_fds[32*32];
        uint8_t spiffs_cache_buf[(LOG_PAGE_SIZE+32)*8];
        //FilesystemHandler(size_t _addr, size_t _size, char * _mountpt);
        FilesystemHandler(char * _mountpt);

};
