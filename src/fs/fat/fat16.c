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

static void fat16_init_private(struct disk* disk, struct fat_private* private){
    memset(private, 0, sizeof(struct fat_private));
    private->cluster_read_stream = diskstreamer_new(disk->id);
    private->fat_read_stream = diskstreamer_new(disk->id);
    private->directory_stream = diskstreamer_new(disk->id);
}

int fat16_sector_to_absolute(struct disk* disk, int sector){
    return sector * disk->sector_size;
}

int fat16_get_total_items_for_directory(struct disk* disk, uint32_t directory_start_sector){
    struct fat_directory_item item;
    struct fat_directory_item empty_item;
    memset(&empty_item, 0, sizeof(empty_item));
    struct fat_private* fat_private = disk->fs_private;
    int response = 0;
    int i = 0;
    int directory_start_pose = directory_start_sector * disk->sector_size;
    // read from start of root directory, the number of items it contains
    struct disk_stream* stream = fat_private->directory_stream;
    if(diskstreamer_seek(stream, directory_start_pose) != BOIOS_ALL_OK) {
        response = -EIO;
        goto out;
    }
    while(1){
        if(diskstreamer_read(stream, &item, sizeof(item)) != BOIOS_ALL_OK){
            response = -EIO;
            goto out;
        }
        // if blank record found, we are done 
        if(item.file_name[0] == 0x00)
            break;
        // if item is unused, skip
        if(item.file_name[5] == 0xE5)
            continue;
        i++;
    }
    response = i;
out:
    return response;
}

int fat16_get_root_directory(struct disk* disk, struct fat_private* fat_private, struct fat_directory* directory){
    int response = 0;
    // root details are extracted from the primary header
    struct fat_header* primary_header = &fat_private->header.primary_header;
    // the equation to get the root directory position on the sector
    int root_dir_sector_pos = (primary_header->fat_copies * primary_header->sectors_per_fat) + primary_header->reserved_sectors;
    int root_dir_entries = fat_private->header.primary_header.root_entries;
    int root_dir_size = root_dir_entries * sizeof(struct fat_directory_item);
    int total_sectors = root_dir_size / disk->sector_size;
    if(root_dir_size % disk->sector_size)
        total_sectors++;
    // get total number of directories from the root directory sector position
    int total_items = fat16_get_total_items_for_directory(disk, root_dir_sector_pos);
    // allocate memory for directory and read the root directory into it
    struct fat_directory_item* dir = kzalloc(root_dir_size);
    if(!dir){
        response = -ENOMEM;
        goto out;
    }
    struct disk_stream* stream = fat_private->directory_stream;
    // first seek the streamer to the root directory position
    if(diskstreamer_seek(stream, fat16_sector_to_absolute(disk, root_dir_sector_pos)) != BOIOS_ALL_OK){
        response = -EIO;
        goto out;
    }
    if(diskstreamer_read(stream, dir, root_dir_size) != BOIOS_ALL_OK){
        response = -EIO;
        goto out;
    }
    directory->item = dir;
    directory->total_items = total_items;
    directory->sector_position = root_dir_sector_pos;
    directory->ending_sector_position = root_dir_sector_pos + (root_dir_size / disk->sector_size);
out:
    return response;
}

int fat16_resolve(struct disk* disk){
    int response = 0;
    // allocate memory for the private fat structure
    struct fat_private* fat_private = kzalloc(sizeof(struct fat_private));
    // initialize private data, bound to the disk, whenever it calls us, we access these private data
    fat16_init_private(disk, fat_private);
    disk->fs_private = fat_private;
    disk->filesystem = &fat16_fs;
    // create a streamer for the disk
    struct disk_stream* stream = diskstreamer_new(disk->id);
    if(!stream){
        response = -ENOMEM;
        goto out;
    }
    // read the first sector of the disk to get the header, and check if the header has the correct signature
    if(diskstreamer_read(stream, &fat_private->header, sizeof(fat_private->header)) != BOIOS_ALL_OK){
        response = -EIO;
        goto out;
    }
    if(fat_private->header.shared.extended_header.signature != 0x29){
        response = -EFSNOTUS;
        goto out;
    }
    // get the root directory for the filesystem
    if(fat16_get_root_directory(disk, fat_private, &fat_private->root_directory) != BOIOS_ALL_OK){
        response = -EIO;
        goto out;
    }
out:
    // close stream after reading
    if(stream)
        diskstreamer_close(stream);
    if(response < 0){
        // unbind the private part from the dik
        kfree(fat_private);
        disk->fs_private = 0;
    }
    return response;
}

void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode){
    return 0;
}
