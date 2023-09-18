/*
File System Manager CLI Program
User Interface for File System: can interact with the disk image including the LS, CAT, and EXPORT commands.
file: command.c
author: Tamsyn Evezard, Andie Perreira, Fatima Nava
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include "utility.h"
#include "file_system.h"


int main( int argc, char *argv[] )  
{
    char* command[10];
    char* filename;
    char* command2[10];
    char* data;

    filename = argv[1];
    printf("Welcome to our virtual file machine. For all your file system needs\n");
    FILE* file = ReadFile(filename);

    MBR* mbr = ReadMBR(file, 0);

    size_t offset_boot;
    for(int i = 0; i < 4; i++)
    {
        if(mbr -> part[i].type == 14)
        {
            offset_boot = mbr -> part[i].lba_offset * 512;
        }
    }

    BOOT* boot = ReadBoot(file, offset_boot);
    size_t offset_FAT = offset_boot + (boot -> reserved_sectors * 512);
    size_t offset_root =  offset_FAT + (2 * boot -> sectors_per_FAT * 512);
    size_t offset_data = offset_root + boot -> root_entries * sizeof(_DIR_ENTRY);
    FAT* fat = ReadFAT(file, boot, offset_FAT);
    bool print_data = false;

    printf("Enter 'ls' 'cat' 'export' or 'stop'\n");
    scanf("%s", command);

    while(strcmp(command, "ls") == 0 || strcmp(command, "cat") == 0 || strcmp(command, "export") == 0)
    {
        if(strcmp(command, "ls") == 0)
        {
            print_data = false;
            ReadRootDir(file, boot, offset_root, fat, offset_data);
            
            printf("Enter 'ls' 'cat' 'export' or 'stop'\n");
            scanf("%s", command);
        }

        else if(strcmp(command, "cat") == 0)
        {
            scanf("%s", command);
            print_data = true;
            data = PrintFile(file, boot, offset_root, fat, offset_data, command);
            printf("%s\n", data);
            printf("Enter 'ls' 'cat' 'export' or 'stop'\n");
            scanf("%s", command);
        }

        else if(strcmp(command, "export") == 0) 
        {
            scanf("%s", command);
            scanf("%s", command2);
            FILE* new_file = fopen(command2, "w"); 
            data = PrintFile(file, boot, offset_root, fat, offset_data, command);
            int size = GetSize(file, boot, offset_root, fat, offset_data, command);
            fwrite(data, 1, GetSize(file, boot, offset_root, fat, offset_data, command), new_file);
            fclose(new_file);
            printf("Enter 'ls' 'cat' 'export' or 'stop'\n");
            scanf("%s", command);
        }
    }

    if(strcmp(command, "stop") == 0 || strcmp(command, "exit") == 0)
    {
        printf("Goodbye :)\n");
        return 0;
    }

    else if(command != "ls" || command != "cat" || command != "export" || command != "stop")
    {
        printf("ERROR: That was not an option, please enter a valid command.\n");
        return 0;
    }
    return 0;
}