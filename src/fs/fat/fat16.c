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
struct fat_file_descriptor{
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
int fat16_read(struct disk* disk, void* descriptor, uint32_t size, uint32_t nmemb, char* out);
int fat16_seek(void* private, uint32_t offset, FILE_SEEK_MODE seek_mode);
int fat16_stat(struct disk* disk, void* private, struct file_stat* stat);
int fat16_close(void* private);

struct filesystem fat16_fs = {
    .resolve = fat16_resolve,
    .open = fat16_open,
    .read = fat16_read,
    .seek = fat16_seek,
    .stat = fat16_stat,
    .close = fat16_close
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
    struct fat_directory_item* dir = 0x00;
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
    dir = kzalloc(root_dir_size);
    if(!dir){
        response = -ENOMEM;
        goto err_out;
    }
    struct disk_stream* stream = fat_private->directory_stream;
    // first seek the streamer to the root directory position
    if(diskstreamer_seek(stream, fat16_sector_to_absolute(disk, root_dir_sector_pos)) != BOIOS_ALL_OK){
        response = -EIO;
        goto err_out;
    }
    if(diskstreamer_read(stream, dir, root_dir_size) != BOIOS_ALL_OK){
        response = -EIO;
        goto err_out;
    }
    directory->item = dir;
    directory->total_items = total_items;
    directory->sector_position = root_dir_sector_pos;
    directory->ending_sector_position = root_dir_sector_pos + (root_dir_size / disk->sector_size);
out:
    return response;
err_out:
    if(dir)
        kfree(dir);
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

void fat16_to_proper_string(char** out, const char* in, size_t size){
    int i=0;
    // go through a string and look for a string or a null terminator
    while(*in != 0x00 && *in != 0x20){
        **out = *in;
        *out += 1;
        in += 1;
        if(i >= size-1)
            break;
        i++;
    }
    // if there is space, add a null terminator, so to reduce the byte used by a file from "FILE    " to "FILE"
    **out = 0x00;
}

void fat16_get_full_relative_filename(struct fat_directory_item* item, char* out, int max_len){
    memset(out, 0x00, max_len);
    char* out_tmp = out;
    fat16_to_proper_string(&out_tmp, (const char*)item->file_name, sizeof(item->file_name));
    // add the extension to the file name
    if(item->ext[0] != 0x00 && item->ext[0] != 0x20){
        *out_tmp++ = '.';
        fat16_to_proper_string(&out_tmp, (const char*)item->ext, sizeof(item->ext));
    }
}

struct fat_directory_item* fat16_clone_directory_item(struct fat_directory_item* item, int size){
    if(size < sizeof(struct fat_directory_item))
        return 0;
    struct fat_directory_item* item_copy = kzalloc(size);
    if(!item_copy)
        return 0;
    memcpy(item_copy, item, size);
    return item_copy;
}

static uint32_t fat16_get_first_cluster(struct fat_directory_item* item){
    return item->high_16_bits_first_cluster | item->low_16_bits_first_cluster;
}

static int fat16_cluster_to_sector(struct fat_private* private, int cluster){
    return private->root_directory.ending_sector_position + ((cluster - 2) * private->header.primary_header.sectors_per_cluster);
}

// first fat comes directly after the reserved sectors
static uint32_t fat16_get_first_fat_sector(struct fat_private* private){
    return private->header.primary_header.reserved_sectors;
}

// go into file allocation table, then pullout the correct fat entry
static int fat16_get_fat_entry(struct disk* disk, int cluster){
    int response = -1;
    struct fat_private* private = disk->fs_private;
    struct disk_stream* stream = private->fat_read_stream;
    if(!stream)
        goto out;
    uint32_t fat_table_position = fat16_get_first_fat_sector(private) * disk->sector_size;
    response = diskstreamer_seek(stream, fat_table_position * (cluster * BOIOS_FAT16_NEW_ENTRY_SIZE));
    if(response < 0)
        goto out;
    uint16_t result = 0;
    response = diskstreamer_read(stream, &result, sizeof(result));
    if(response < 0)
        goto out;
    response = result;
out:
    return response;
}

// get the correct cluster based on the starting cluster and offset
static int fat16_get_cluster_for_offset(struct disk* disk, int starting_cluster, int offset){
    int response = 0;
    struct fat_private* private = disk->fs_private;
    int size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = starting_cluster;
    int clusters_ahead = offset / size_of_cluster_bytes;
    for(int i=0; i<clusters_ahead; i++){
        int entry = fat16_get_fat_entry(disk, cluster_to_use);
        // if we are at last entry
        if(entry == 0xFF8 || entry == 0xFFF){
            response = -EIO;
            goto out;
        }
        // check if sector is bad sector
        if(entry == BOIOS_FAT16_BAD_SECTOR){
            response = -EIO;
            goto out;
        }
        // check if sector is reserved
        if(entry == 0xFF0 || entry == 0xFF6){
            response = -EIO;
            goto out;
        }
        // check if fat is corrupted
        if(entry == 0x00){
            response = -EIO;
            goto out;
        }
        cluster_to_use = entry;
    }
    response = cluster_to_use;
out:
    return response;
}

static int fat16_read_internal_from_stream(struct disk* disk, struct disk_stream* stream, int cluster, int offset, int total, void* out){
    int response = 0;
    struct fat_private* private = disk->fs_private;
    int size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = fat16_get_cluster_for_offset(disk, cluster, offset);
    if(cluster_to_use < 0){
        response = cluster_to_use;
        goto out;
    }
    int offset_from_cluster = offset % size_of_cluster_bytes;
    int starting_sector = fat16_cluster_to_sector(private, cluster_to_use);
    int starting_pos = (starting_sector * disk->sector_size) + offset_from_cluster;
    int total_to_read = total > size_of_cluster_bytes ? size_of_cluster_bytes : total;
    response = diskstreamer_seek(stream, starting_pos);
    if(response != BOIOS_ALL_OK) 
        goto out;
    response = diskstreamer_read(stream, out, total_to_read);
    if(response != BOIOS_ALL_OK)
        goto out;
    total -= total_to_read;
    // if we still have more to read
    if(total > 0)
        response = fat16_read_internal_from_stream(disk, stream, cluster, offset + total_to_read, total, out + total_to_read);
out:
    return response;
}

static int fat16_read_internal(struct disk* disk, int starting_cluster, int offset, int total, void* out){
    struct fat_private* fs_private = disk->fs_private;
    struct disk_stream* stream = fs_private->cluster_read_stream;
    return fat16_read_internal_from_stream(disk, stream, starting_cluster, offset, total, out);
}

void fat16_free_directory(struct fat_directory* directory){
    if(!directory)
        return;
    if(directory->item)
        kfree(directory->item);
    kfree(directory);
}

void fat16_fat_item_free(struct fat_item* item){
    if(item->type == FAT_ITEM_TYPE_DIRECTORY)
        fat16_free_directory(item->directory);
    else if(item->type == FAT_ITEM_TYPE_FILE)
        kfree(item->item);
    kfree(item);
}

struct fat_directory* fat16_load_fat_directory(struct disk* disk, struct fat_directory_item* item){
    int response = 0;
    struct fat_directory* directory = 0;
    struct fat_private* fat_private = disk->fs_private;
    // if a file is passed, return an error we want to load a directory not a file
    if(!(item->attributes & FAT_FILE_SUBDIRECTORY)){
        response = -EINVARG;
        goto out;
    }
    directory = kzalloc(sizeof(struct fat_directory));
    if(!directory){
        response = -ENOMEM;
        goto out;
    }
    // calculate total items in cluster
    int cluster = fat16_get_first_cluster(item);
    int cluster_sector = fat16_cluster_to_sector(fat_private, cluster);
    int total_items = fat16_get_total_items_for_directory(disk, cluster_sector);
    directory->total_items = total_items;
    // subdirectories are an array of fat directory items
    int directory_size = directory->total_items * sizeof(struct fat_directory_item);
    directory->item = kzalloc(directory_size);
    if(!directory->item){
        response = -ENOMEM;
        goto out;
    }
    // read the fat directory
    response = fat16_read_internal(disk, cluster, 0x00, directory_size, directory->item);
    if(response != BOIOS_ALL_OK)
        goto out;
out:
    if(response != BOIOS_ALL_OK)
        fat16_free_directory(directory);
    return directory;
}

struct fat_item* fat16_new_fat_item_or_directory_item(struct disk* disk, struct fat_directory_item* item){
    struct fat_item* f_item = kzalloc(sizeof(struct fat_item));
    if(!f_item)
        return 0;
    // first check if the item is a directory
    if(item->attributes & FAT_FILE_SUBDIRECTORY){
        f_item->directory = fat16_load_fat_directory(disk, item);
        f_item->type = FAT_ITEM_TYPE_DIRECTORY;
        return f_item;
    }
    f_item->type = FAT_ITEM_TYPE_FILE;
    // clone item to avoid any problems with memory of given item
    f_item->item = fat16_clone_directory_item(item, sizeof(struct fat_directory_item)); 
    return f_item;
}

struct fat_item* fat16_find_item_in_directory(struct disk* disk, struct fat_directory* directory, const char* file_name){
    struct fat_item* f_item = 0;
    char tmp_filename[BOIOS_MAX_PATH];
    // loop over all items in directory, resolve full relative filename
    for(int i=0; i<directory->total_items; i++){
        fat16_get_full_relative_filename(&directory->item[i], tmp_filename, sizeof(tmp_filename));
        // if found, create a new fat item
        if(istrncmp(tmp_filename, file_name, sizeof(tmp_filename)) == 0){
            f_item = fat16_new_fat_item_or_directory_item(disk, &directory->item[i]);
        }
    }
    return f_item;
}

struct fat_item* fat16_get_directory_entry(struct disk* disk, struct path_part* path){
    struct fat_private* fat_private = disk->fs_private;
    struct fat_item* current_item = 0;
    // first get the root item from the given oath
    struct fat_item* root_item = fat16_find_item_in_directory(disk, &fat_private->root_directory, path->part_name);
    if(!root_item)
        goto out;
    // check if there is a next part, aka child directory
    struct path_part* next_part = path->next;
    current_item = root_item;
    // if there is no child directory, return the current directory
    while(next_part != 0){
        // check if current item is a directory first 
        if(current_item->type != FAT_ITEM_TYPE_DIRECTORY){
            current_item = 0;
            break;
        }
        // if item is a directory, find next item, then free the old one
        struct fat_item* tmp_item = fat16_find_item_in_directory(disk, current_item->directory, next_part->part_name);
        fat16_fat_item_free(current_item);
        current_item = tmp_item;
        next_part = next_part->next;
    }
out:
    return current_item;
}

void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode){
    struct fat_file_descriptor* descriptor = kzalloc(sizeof(struct fat_file_descriptor));
    int err_code = 0;
    if(mode != FILE_MODE_READ){
        err_code = -ERDONLY;
        goto err_out;
    }
    if(!descriptor){
        err_code = -ENOMEM;
        goto err_out;
    }
    // get the fat16 item from path
    descriptor->item = fat16_get_directory_entry(disk, path);
    if(!descriptor->item){
        err_code = -EIO;
        goto err_out;
    }
    // when file is first opened, stream is always at first byte of the file
    descriptor->position = 0;
    return descriptor;
err_out:
    if(descriptor)
        kfree(descriptor);
    return ERROR(err_code);
}

int fat16_stat(struct disk* disk, void* private, struct file_stat* stat){
    int response = 0;
    struct fat_file_descriptor* desc = (struct fat_file_descriptor*)private;
    struct fat_item* desc_item = desc->item;
    if(desc_item->type != FAT_ITEM_TYPE_FILE){
        response = -EINVARG;
        goto out;
    }
    // access privcate data and get file size from it
    struct fat_directory_item* ritem = desc_item->item;
    stat->file_size = ritem->file_size;
    stat->flags = 0x00;
    if(ritem->attributes & FAT_FILE_READ_ONLY)
        stat->flags |= FILE_STAT_READ_ONLY;
out:
    return response;
}

int fat16_read(struct disk* disk, void* descriptor, uint32_t size, uint32_t nmemb, char* out){
    int response = 0;
    struct fat_file_descriptor* fat_desc = descriptor;
    struct fat_directory_item* item = fat_desc->item->item;
    int offset = fat_desc->position;
    for(uint32_t i=0; i<nmemb; i++){
        response = fat16_read_internal(disk, fat16_get_first_cluster(item), offset, size, out);
        if(ISERR(response))
            goto out;
        out += size;
        offset += size;
    }
    // response should be equal to the total read
    response = nmemb;
out:
    return response;
}

int fat16_seek(void* private, uint32_t offset, FILE_SEEK_MODE seek_mode){
    int response = 0;
    struct fat_file_descriptor* desc = private;
    struct fat_item* desc_item = desc->item;
    if(desc_item->type != FAT_ITEM_TYPE_FILE){
        response = -EINVARG;
        goto out;
    }
    struct fat_directory_item* ritem = desc_item->item;
    // make sure user is not seeking past file size
    if(offset >= ritem->file_size){
        response = -EIO;
        goto out;
    }
    switch(seek_mode){
        case SEEK_SET: desc->position = offset; break;
        case SEEK_END: response = -EUNIMP; break;
        case SEEK_CUR: desc->position += offset; break;
        default: response = -EINVARG; break;
    }
out:
    return response;
}

static void fat16_free_file_descriptor(struct fat_file_descriptor* desc){
    fat16_fat_item_free(desc->item);
    kfree(desc);
}

int fat16_close(void* private){
    fat16_free_file_descriptor((struct fat_file_descriptor*)private);
    return 0;
}