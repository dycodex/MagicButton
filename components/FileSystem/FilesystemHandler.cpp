#include "FilesystemHandler.h"

FilesystemHandler *FilesystemHandler::instance = nullptr;

extern "C"
{
	#include <esp_partition.h>
    #include "esp32_fs.h"
    #include "esp_spiffs.h"
	#include "esp_log.h"
}


//FilesystemHandler *FilesystemHandler::get_instance(size_t _addr, size_t _size, char * _mountpt)
FilesystemHandler *FilesystemHandler::get_instance(char * _mountpt)
{
    if(FilesystemHandler::instance == nullptr)
    {
        //FilesystemHandler::instance = new FilesystemHandler(_addr, _size, _mountpt);
    	FilesystemHandler::instance = new FilesystemHandler(_mountpt);
    }
    return FilesystemHandler::instance;
}


//FilesystemHandler::FilesystemHandler(size_t _addr, size_t _size, char * _mountpt)
//:addr(_addr), size(_size), mountpt(_mountpt)
FilesystemHandler::FilesystemHandler(char * _mountpt)
:mountpt(_mountpt)
{
}

bool FilesystemHandler::init_spiffs()
{

	const esp_partition_t* partition = esp_partition_find_first(
			ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, mountpt);

	if (partition) {
		ESP_LOGI("SPIFFS", "partition found at address %08x of size %u",
				partition->address, partition->size);
	} else {
		ESP_LOGE("SPIFFS", "SPIFFS partition named '%s' NOT found", mountpt);
		return false;
	}

    //Configure the blocksizes and addresses of the spiffs.
    //This should correlate with the settings in the makefile and partitions.csv
	spiffs_config cfg;
    int rc;
//	cfg.phys_size = this->size;
//	cfg.phys_addr = this->addr;
    cfg.phys_size = partition->size;
    cfg.phys_addr = partition->address;
	cfg.phys_erase_block = 65536;
	cfg.log_block_size = 65536;
	cfg.log_page_size = LOG_PAGE_SIZE;

    //Setup spi flash functions
	cfg.hal_read_f = esp32_spi_flash_read;
	cfg.hal_write_f = esp32_spi_flash_write;
    cfg.hal_erase_f = esp32_spi_flash_erase;

    //Try to mount
    printf("Loading SPIFFS at address 0x%x\n", cfg.phys_addr);
	rc = SPIFFS_mount(&this->fs,
		&cfg,
        this->spiffs_work_buf,
		this->spiffs_fds,
		sizeof(this->spiffs_fds),
		this->spiffs_cache_buf,
		sizeof(this->spiffs_cache_buf),
        0);
	if (rc != 0) {
        printf("\n###################################################################\n");
        printf("Error mounting SPIFFS! (have you flashed it to the correct address?)\n");
        printf("This was the SPIFFS settings used:\n");
        printf("Address: 0x%x\n", cfg.phys_addr);
        printf("Size: 0x%x\n", cfg.phys_size);
        printf("Mount point: %s\n", this-> mountpt);
        printf("Error id: %d\n", rc);
        printf("This will probably cause major problems.\n");
        printf("###################################################################\n\n");
        printf("We will try to format the filesystem\n");

        //The below uncommented code is mostly for debugging.
        //It is probably not the best idea to format the filesystem when encountering an error.

        //rc = SPIFFS_format(&this->fs);
        //printf("Format returned errorcode: %d\n", rc);
        return false;
    }

	char vfsMp[128];
	snprintf(vfsMp, sizeof vfsMp, "/%s", this->mountpt);

    spiffs_registerVFS((char*)vfsMp, &this->fs);
    printf("File system initialization completed!\n");
    return true;
}

void FilesystemHandler::test() { //TODO REMOVE TEST METHOD

	char buf[12];

	// Surely, I've mounted spiffs before entering here

	spiffs_file fd;
	fd = SPIFFS_open(&fs, "my_file",
	SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
	if (SPIFFS_write(&fs, fd, (u8_t *) "Hello world!", 12) < 0)
		printf("errno %i\n", SPIFFS_errno(&fs));
	SPIFFS_close(&fs, fd);

	fd = SPIFFS_open(&fs, "my_file", SPIFFS_RDWR, 0);
	if (SPIFFS_read(&fs, fd, (u8_t *) buf, 12) < 0)
		printf("errno %i\n", SPIFFS_errno(&fs));
	SPIFFS_close(&fs, fd);

	printf("--> %s <--\n", buf);
}
