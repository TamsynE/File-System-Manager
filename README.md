# File-System-Manager

# Usage
Group Project: 3 Members

The core of this project is a library that allows you to interact and manipulate a virtual file system. Specifically, it reads a isk image and summarizes the partitions within the MBR, and lists the contents of the root directory. In addition, the command line program can interact with the disk image using the LS, CAT, and EXPORT commands.

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
- include "utility.h"
- include "file_system.h"

# File System Manager Functions
- void CreateMemoryManager(int sizeTotal, bool bf)
- void* mallocMem(int size, bool bf)
- NODE* CheckBusyList(void* data)
- NODE* CheckFreeListBefore(NODE* curr)
- NODE* CheckFreeListAfter(NODE* curr)
- void* freeMem(void* data)
- void DumpFree()
- void DumpBusy()
- void DestroyMem()
