#include "fat16.h"

#define BOIOS_FAT16_SIGNATURE       0X29
#define BOIOS_FAT16_NEW_ENTRY_SIZE  0X02
#define BOIOS_FAT16_BAD_SECTOR      0XFF7
#define BOIOS_FAT16_UNUSED          0X00

// fat directory entry attributes bitmask
#define FAT_FILE_READ_ONLY  0X01
#define FAT_FILE_HIDDEN     0X02
#define FAT_FILE_SYSTEM     0X04
#define FAT_FILE_ARCHIVED   0X20
#define FAT_FILE_DEVICE     0X40
#define FAT_FILE_RESERVED   0X80
#define FAT_FILE_VOLUME_LABEL  0X08
#define FAT_FILE_SUBDIRECTORY  0X10

#define FAT_ITEM_TYPE_DIRECTORY 0
#define FAT_ITEM_TYPE_FILE      1

// possible values are: FAT_ITEM_TYPE_FILE and FAT_ITEM_TYPE_DIRECTORY 
typedef unsigned int FAT_ITEM_TYPE;

struct fat_header_extended{
    uint8_t drive_no;
    uint8_t win_nt_bit;
    uint8_t signature;
    uint32_t volume_id;
    uint8_t volume_id_string[11];
    uint8_t system_id_string[8];
} __attribute__((packed));

struct fat_header{
    uint8_t short_jmp_ins[3];
    uint8_t oem_id[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_copies;
    uint16_t root_entries;
    uint16_t number_of_sectors;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_sectors;
    uint32_t sectors_big;
} __attribute__((packed));

struct fat_h{
    struct fat_header primary_header;
    union fat_h_e{
        struct fat_header_extended extended_header;
    } shared;

};

struct fat_directory_item{
    uint8_t file_name[8];
    uint8_t ext[3];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t creation_time_tenths_of_a_sec;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access;
    uint16_t high_16_bits_first_cluster;
    uint16_t last_mode_time;
    uint16_t last_mod_date;
    uint16_t low_16_bits_first_cluster;
    uint32_t file_size;
} __attribute__((packed));

struct fat_directory{
    struct fat_directory_item* item; // will point to first item in directory
    int total_items;
    int sector_position;
    int ending_sector_position;
};

struct fat_item{
    union{
        struct fat_directory_item* item; // if it is a normal file, acces this
        struct fat_directory* directory; // otherwise acces this
    };
    FAT_ITEM_TYPE type;
};

// represents an open file
struct fat_item_descriptor{
    struct fat_item* item;
    uint32_t position;
};

struct fat_private{
    struct fat_h header;
    struct fat_directory root_directory;
    // used to stream data clusters
    struct disk_stream* cluster_read_stream;
    // used to stream FAT
    struct disk_stream* fat_read_stream;
    // used when directory is streamed
    struct disk_stream* directory_stream;
};

int fat16_resolve(struct disk* disk);
void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode);

struct filesystem fat16_fs = {
    .resolve = fat16_resolve,
    .open = fat16_open
};

struct filesystem* fat16_init(){
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}

int fat16_resolve(struct disk* disk){
    return 0;
}

void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode){
    return 0;
}
