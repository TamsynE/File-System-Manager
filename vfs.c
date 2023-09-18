/*
File System Manager
Reads the Master Boot Record and identify any partitions that are the correct File System. 
Parses the File System's Boot Record and File Allocation Table and use them to navigate the drive.
Displays the contents of the Root Directory.
file: vfs.c
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
#include "utility.h"
#include "file_system.h"

static char g_working_directory[256] = {0};
static FILE* g_file_pointer = NULL;
static FAT* g_partition_list = NULL;
static int g_partition_count = 0;
bool print_data = false;

uint16_t FindNextCluster(FAT* fat, uint16_t cluster);
char* ReadData(uint16_t cluster, FAT* fat, _DIR_ENTRY* dir, size_t offset_data, uint8_t sectors_per_cluster, FILE* fp);

FILE* ReadFile(const char* filename)
{
    struct stat fs = {0};
    int rc = stat(filename, &fs);
    FILE* fp = fopen(filename, "rb");
    
    if(fp == NULL)
    {
        printf("ERROR: file does not exist\n");
        return NULL;
    }
    return fp;
}

MBR* ReadMBR(FILE* fp, long int offset)
{
    // use fseek to the beginning of file 
    fseek(fp, offset, SEEK_SET);
    // read entire MBR sector
    char* buffer = (char*)malloc(sizeof(MBR));
    if(buffer == NULL)
    {
        printf("Error\n");
        return NULL;
    }

    memset(buffer, 0, sizeof(MBR));
    size_t count = fread(buffer, sizeof(char), sizeof(MBR), fp);
    if (count != sizeof(MBR)) 
    {
        printf("ERROR\n");
        free(buffer);
        return NULL;
    }
    MBR* mbr = (MBR*)buffer;

    return mbr;
}

// read boot
BOOT* ReadBoot(FILE* fp, size_t offset)
{
    fseek(fp, offset, SEEK_SET);
    char* buffer = (char*)malloc(sizeof(BOOT));
    if(buffer == NULL)
    {
        printf("ERROR\n");
        return NULL;
    }

    memset(buffer, 0, sizeof(BOOT));
    size_t count = fread(buffer, sizeof(char), sizeof(BOOT), fp);
    if (count != sizeof(BOOT)) 
    {
        printf("ERROR\n");
        free(buffer);
        return NULL;
    }
    BOOT* boot = (BOOT*)buffer;

    return boot;
}

// read FAT table
FAT* ReadFAT(FILE* fp, BOOT* boot, size_t offset)
{
    fseek(fp, offset, SEEK_SET);

    //Read the entire Root Directory at once
    char* buffer = (char*)malloc(boot -> sectors_per_FAT * 512);
    memset(buffer, 0, boot -> sectors_per_FAT * 512);
    size_t count = fread(buffer, sizeof(char), boot -> sectors_per_FAT * 512, fp);
            
    if (count != boot -> sectors_per_FAT * 512)    
    {
        printf("ERROR\n");
        free(buffer);
        return NULL;
    }      

    FAT* fat = (FAT*)buffer;
    return fat;
}

// get directories and files in file
void ReadRootDir(FILE* fp, BOOT* boot, size_t offset, FAT* fat, size_t offset_data)
{
    fseek(fp, offset, SEEK_SET);

    char* buffer = (char*)malloc(sizeof(_DIR_ENTRY) * boot -> root_entries);
    memset(buffer, 0, sizeof(_DIR_ENTRY) * boot -> root_entries);
    size_t count = fread(buffer, sizeof(char), sizeof(_DIR_ENTRY) * boot -> root_entries, fp);
            
    if (count != sizeof(_DIR_ENTRY) * boot -> root_entries)    
    {
        printf("ERROR\n");
        free(buffer);
    }      

    _DIR_ENTRY* dir = (_DIR_ENTRY*)buffer;

    while(dir -> name8[0] != NULL) // gotten to first non-entry - exit loop
    {
        char* helper = EightDotThreeString(dir -> name8, dir -> ext3);

        uint8_t DIR_MASK = 0x10;
        uint8_t is_directory = dir -> attribute & DIR_MASK;
        uint8_t is_lfn = dir -> attribute & FILE_ATTRIBUTE_LFN;
        uint8_t vol = dir -> attribute & FILE_ATTRIBUTE_VOLUME;
                
        if(is_lfn != FILE_ATTRIBUTE_LFN) // not long file name
        {
            uint16_t cluster = dir -> firstcluster;

            uint16_t next_cluster = FindNextCluster(fat, cluster);

            while(next_cluster < 0xFFF8 && next_cluster != 0x0000)
            {
                next_cluster = FindNextCluster(fat, next_cluster);
            }
            
            if(vol == FILE_ATTRIBUTE_VOLUME)
            {

            }
            
            else if(is_directory == 0)
            {

                printf("F: %s\n", helper);
            }

            else 
            {
                printf("D: %s\n", helper);
            }
        }

        dir++;
    }
}

// get file contents - used primarily for cat command
char* PrintFile(FILE* fp, BOOT* boot, size_t offset, FAT* fat, size_t offset_data, char* f_name)
{
    fseek(fp, offset, SEEK_SET);

    char* buffer = (char*)malloc(sizeof(_DIR_ENTRY) * boot -> root_entries);
    memset(buffer, 0, sizeof(_DIR_ENTRY) * boot -> root_entries);
    size_t count = fread(buffer, sizeof(char), sizeof(_DIR_ENTRY) * boot -> root_entries, fp);
            
    if (count != sizeof(_DIR_ENTRY) * boot -> root_entries)    
    {
        printf("ERROR\n");
        free(buffer);
    }      

    _DIR_ENTRY* dir = (_DIR_ENTRY*)buffer;

    while(dir -> name8[0] != NULL) // gotten to first non-entry - exit loop
    {
        char* helper = EightDotThreeString(dir -> name8, dir -> ext3);

        uint8_t DIR_MASK = 0x10;
        uint8_t is_directory = dir -> attribute & DIR_MASK;
        uint8_t is_lfn = dir -> attribute & FILE_ATTRIBUTE_LFN;
        uint8_t vol = dir -> attribute & FILE_ATTRIBUTE_VOLUME;
                
        if(is_lfn != FILE_ATTRIBUTE_LFN) // not long file name
        {
            uint16_t cluster = dir -> firstcluster;

            uint16_t next_cluster = FindNextCluster(fat, cluster);

            while(next_cluster < 0xFFF8 && next_cluster != 0x0000)
            {
                next_cluster = FindNextCluster(fat, next_cluster);
            }

            if(is_directory == 0)
            {
                if(strcmp(f_name, helper) == 0)
                {
                    return ReadData(dir -> firstcluster, fat, dir, offset_data, boot -> sectors_per_cluster, fp);
                }
            }
        }

        dir++;
    }
    return NULL;
}

uint16_t FindNextCluster(FAT* fat, uint16_t cluster)
{
    uint16_t next_cluster = fat -> table[cluster];
    return next_cluster;
}

// Read the data in file (used in ReadRootDir)
char* ReadData(uint16_t cluster, FAT* fat, _DIR_ENTRY* dir, size_t offset_data, uint8_t sectors_per_cluster, FILE* fp)
{
    size_t total_bytes = sectors_per_cluster * 512;
    int count = 0;
    char* data;
    int bytes_read = 0;

    if(dir -> sizefile >= total_bytes)
    {
        char* buffer = (char*)malloc(dir -> sizefile);
        char* new_buffer = buffer + total_bytes * count;
        uint16_t cluster = dir -> firstcluster;
        size_t new_data_offset = offset_data + total_bytes * (cluster - 2);
        fseek(fp, new_data_offset, SEEK_SET);
        
        if(count > 0)
        {
            bytes_read += fread(new_buffer, sizeof(char), total_bytes, fp);
        }

        else 
        {
            bytes_read += fread(buffer, sizeof(char), total_bytes, fp);
        }
        
        uint16_t next_cluster = FindNextCluster(fat, cluster);

        while(next_cluster < 0xFFF8 && next_cluster != 0x0000)
        {
            size_t new_data_offset = offset_data + total_bytes * (next_cluster - 2);
            fseek(fp, new_data_offset, SEEK_SET); //offset_cluster
            bytes_read += fread((new_buffer + bytes_read), sizeof(char), total_bytes, fp);
            next_cluster = FindNextCluster(fat, next_cluster);
        }
        count++;
        data = buffer;
    }
    
    else
    {
        size_t new_data_offset = offset_data + total_bytes * (cluster - 2);
        fseek(fp, new_data_offset, SEEK_SET);
        char* buffer = (char*)malloc(dir -> sizefile);
        memset(buffer, 0, dir -> sizefile);
        size_t count = fread(buffer, sizeof(char), dir -> sizefile, fp);
        data = buffer;
    }
    return data;
}

// get partitions & types (FAT16/32) from MBR
void PrintMBR(MBR* mbr) 
{
    printf("Boot code: %x - \n", mbr -> bootcode[0]); 

    for(int i = 0; i < 4; i++)
    {
        // FAT16 - type 14
        if(mbr -> part[i].type == 14)
        {
            printf("FAT16:\n");
        }
        
        //FAT32 - type 12
        if(mbr -> part[i].type == 12)
        {
            printf("FAT32:\n");
        }

        printf("Partition %d: %x - %x | %d\n", i, mbr -> part[i].lba_offset*512, mbr -> part[i].sector_count*512, mbr -> part[i].type);
        
    }
    printf("Magic:%x - \n", mbr -> magic);
}

// get size of file - used for export command
size_t GetSize(FILE* fp, BOOT* boot, size_t offset, FAT* fat, size_t offset_data, char* f_name)
{
    fseek(fp, offset, SEEK_SET);
    char* buffer = (char*)malloc(sizeof(_DIR_ENTRY) * boot -> root_entries);
    memset(buffer, 0, sizeof(_DIR_ENTRY) * boot -> root_entries);
    size_t count = fread(buffer, sizeof(char), sizeof(_DIR_ENTRY) * boot -> root_entries, fp);
            
    if (count != sizeof(_DIR_ENTRY) * boot -> root_entries)    
    {
        printf("ERROR\n");
        free(buffer);
    }      

    _DIR_ENTRY* dir = (_DIR_ENTRY*)buffer;

    while(dir -> name8[0] != NULL) // gotten to first non-entry - exit loop
    {
        char* helper = EightDotThreeString(dir -> name8, dir -> ext3);

        uint8_t DIR_MASK = 0x10;
        uint8_t is_directory = dir -> attribute & DIR_MASK;
        uint8_t is_lfn = dir -> attribute & FILE_ATTRIBUTE_LFN;
        uint8_t vol = dir -> attribute & FILE_ATTRIBUTE_VOLUME;
                
        if(is_lfn != FILE_ATTRIBUTE_LFN) // not long file name
        {
            if(is_directory == 0)
            {
                if(strcmp(f_name, helper) == 0)
                {
                    return dir -> sizefile;
                }
            }
        }

        dir++;
    }
    return -1;
}