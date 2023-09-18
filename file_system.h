#ifndef file_system_h
#define file_system_h

#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

typedef struct _PARTITION
{
    uint8_t bootable; 
    uint8_t first_chs[3]; 
    uint8_t type; 
    uint8_t last_chs[3];
    uint32_t lba_offset; 
    uint32_t sector_count; 
    
}__attribute__((packed)) PARTITION;

typedef struct MBR
{
    uint8_t bootcode[446];
    PARTITION part[4]; 
    uint16_t magic;

}__attribute__((packed)) MBR;

typedef struct FAT
{
    uint16_t table[1]; 

}__attribute__((packed)) FAT;

typedef struct BOOT
{
    uint8_t jump[3];
    uint8_t nameOEM[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t noFATs;
    uint16_t root_entries;
    uint16_t small_sectors;
    uint8_t media_type;
    uint16_t sectors_per_FAT;
    uint16_t sectors_per_track;
    uint16_t no_heads;
    uint32_t hidden_sectors;
    uint32_t large_sectors;
    uint8_t physical_disk_no;
    uint8_t current_head;
    uint8_t signature;
    uint8_t volume_serial_no[4];
    uint8_t volume_label[11];
    uint8_t system_ID[8];
    uint8_t bootstrap[448];
    uint8_t magic[2];
}__attribute__((packed)) BOOT;

typedef struct _DIR_ENTRY
{
    uint8_t name8[8];
    uint8_t ext3[3];
    uint8_t attribute; 
    uint8_t ctnmilli[2]; 
    uint8_t ctntime[2];
    uint8_t ctndate[2];
    uint8_t lastaccessed[2];
    uint8_t reserved[2];
    uint8_t modtime[2]; 
    uint8_t moddate[2];
    uint16_t firstcluster;
    uint32_t sizefile;

}__attribute__((packed)) _DIR_ENTRY;

uint16_t FindNextCluster(FAT* fat, uint16_t cluster);
char* ReadData(uint16_t cluster, FAT* fat, _DIR_ENTRY* dir, size_t offset_data, uint8_t sectors_per_cluster, FILE* fp);
FILE* ReadFile(const char* filename);
MBR* ReadMBR(FILE* fp, long int offset);
BOOT* ReadBoot(FILE* fp, size_t offset);

FAT* ReadFAT(FILE* fp, BOOT* boot, size_t offset);
void ReadRootDir(FILE* fp, BOOT* boot, size_t offset, FAT* fat, size_t offset_data);
void PrintMBR(MBR* mbr);
size_t GetSize(FILE* fp, BOOT* boot, size_t offset, FAT* fat, size_t offset_data, char* f_name);
char* PrintFile(FILE* fp, BOOT* boot, size_t offset, FAT* fat, size_t offset_data, char* f_name);

#endif