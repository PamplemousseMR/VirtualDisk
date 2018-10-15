#ifndef __VIRTUALDISK_H__
#define __VIRTUALDISK_H__

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#include "tools.h"
#include "block.h"

typedef char header;
typedef int address;
typedef char filename[32 - sizeof(char) - sizeof(int)];

static const int _RECORDING_ = sizeof(header) + sizeof(filename) + sizeof(address);
static const int _MIN_BLOC_LINE_ = 16;
static const int _MIN_BLOC_SIZE_ = 512;
static const int _MIN_BLOC_NUMBER_ = 32;
static const int _HEADER_SIZE_ = sizeof(header);
static const int _NAME_SIZE_ = sizeof(filename);
static const int _ADDRESS_SIZE_ = sizeof(address);
static const char _HEADER_DIRECTORY_ = 'd';
static const char _HEADER_FILE_ = 'f';

typedef struct {

	int nbBlocs;
	int blocSize;
	int currentDirectory;
	int* bitmap;
	bloc_t** blocs;
    
} disk_t;

disk_t* createDisk(int, int);
disk_t* createFromFileDisk(char*);
int destroyDisk(disk_t*);
int displayDisk(disk_t*);
int displayDirectoryDisk(disk_t*);
int checkFile(disk_t*,char*);
int saveDisk(disk_t*,char*);
void pwdDisk(disk_t*);
void printFileContentDisk(disk_t*, char*);
int addDirectoryDisk(disk_t*,char*);
int addFileDisk(disk_t*,char*);
int moveToDirectory(disk_t*,char*);
int removeFile(disk_t*, char*);
void rmdirDisk(disk_t*, char*);
void displayTreeDisk(disk_t* d,bloc_t* b ,char* ,int* ,int);
void addToFileDisk(disk_t*, char*, char*);

#endif
