# File-System-Manager

# Usage
Group Project: 3 Members

The core of this project is a library that allows you to interact and manipulate a virtual file system.

It performs the following functions:
1. Read a disk image and summarize the partitions within the MBR.
2. Read a disk image and list the contents of the root directory.
3. Create a command line program that can interact with the disk image including the LS, CAT, and EXPORT commands.
4. Add the ability to navigate the file system with CD and PWD commands
5. Add the ability to manipulate the file system with CP, MV, and IMPORT commands

# Installation
Ensure that all files are in the same folder. Enter the file the project is in.  

Compiling and running the Memory Manager Test Program:
- gcc ll.c mm.c mmtest.c -o mm.exe  
- ./mm.exe

Compiling and running the Command Line Program:
- gcc ll.c mm.c cli.c -o cli.exe
- ./cli.exe

# Dependencies
- include <math.h>
- include <stdio.h>
- include <stdlib.h>
- include <stdbool.h>
- include "project1.h"
- include "project2.h"

# Memory Manager Functions
- void CreateMemoryManager(int sizeTotal, bool bf)
- void* mallocMem(int size, bool bf)
- NODE* CheckBusyList(void* data)
- NODE* CheckFreeListBefore(NODE* curr)
- NODE* CheckFreeListAfter(NODE* curr)
- void* freeMem(void* data)
- void DumpFree()
- void DumpBusy()
- void DestroyMem()


# Command Line Program
The command line program allows a user to simulate an operating system. 

The program has commands for:
- allocating by size
- freeing by address
- printing busy list
- printing free list
- destroying memory manager
