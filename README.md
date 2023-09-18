# File-System-Manager

# Usage
Group Project: 3 Members

The core of this project is a library that allows you to interact and manipulate a virtual file system. Specifically, it reads a disk image, summarizes the partitions within the MBR, and lists the contents of the root directory. In addition, the command line program can interact with the disk image using the LS, CAT, and EXPORT commands.

# Installation
Ensure that all files are in the same folder. Enter the file the project is in.  

Compiling and running the Command Line Program with the File System Manager:
- gcc command.c vfs.c utility.c -o command.exe
- ./command.exe

# Dependencies
- include <sys/types.h>
- include <sys/stat.h>
- include <stdbool.h>
- include <unistd.h>
- include <stdint.h>
- include <stdio.h>
- include <stdlib.h>
- include <string.h>
- include <dirent.h>
- include <ctype.h>
- include <memory.h>
- include <assert.h>
- include "utility.h"
- include "file_system.h"

# File System Manager Functions
- uint16_t FindNextCluster(FAT* fat, uint16_t cluster)
- char* ReadData(uint16_t cluster, FAT* fat, _DIR_ENTRY* dir, size_t offset_data, uint8_t sectors_per_cluster, FILE* fp)
- FILE* ReadFile(const char* filename)
- MBR* ReadMBR(FILE* fp, long int offset)
- BOOT* ReadBoot(FILE* fp, size_t offset)
- FAT* ReadFAT(FILE* fp, BOOT* boot, size_t offset)
- void ReadRootDir(FILE* fp, BOOT* boot, size_t offset, FAT* fat, size_t offset_data)
- void PrintMBR(MBR* mbr)
- size_t GetSize(FILE* fp, BOOT* boot, size_t offset, FAT* fat, size_t offset_data, char* f_name)
- char* PrintFile(FILE* fp, BOOT* boot, size_t offset, FAT* fat, size_t offset_data, char* f_name);\
