#include "file.h"

struct filesystem* filesystems[BOIOS_MAX_FILESYSTEMS];
struct file_descriptor* file_descriptors[BOIOS_MAX_FILE_DESCRIPTORS];

// get a free filesystem slot
static struct filesystem** fs_get_free_filesystem(){
    for(int i = 0; i<BOIOS_MAX_FILESYSTEMS; i++){
        if(filesystems[i] == 0)
            return &filesystems[i];
    }
    // when there is no free filesystem slot
    return 0;
}

// load static filesystems, built in the core kernel
static void fs_static_load(){
    // fs_insert_filesystem(fat16_init());
}

static int file_new_descriptor(struct file_descriptor** desc_out){
    int response = -ENOMEM;
    for(int i=0; i<BOIOS_MAX_FILE_DESCRIPTORS; i++){
        if(file_descriptors[i] == 0){
            struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));
            // descriptors start at 1
            desc->index = i + 1;
            file_descriptors[i] = desc;
            *desc_out = desc;
            response = 0;
            break;
        }
    }
    return response;
}

static struct file_descriptor* file_get_descriptor(int fd){
    if(fd <=0 || fd >=BOIOS_MAX_FILE_DESCRIPTORS)
        return 0;
    // descriptors start at 1
    int index = fd - 1;
    return file_descriptors[index];
}

void fs_insert_filesystem(struct filesystem* filesystem){
    struct filesystem** fs = fs_get_free_filesystem();
    if(!fs){
        print("Problem inserting filesystem");
        while(1);
    }
    // set value of the empty array to the given filesystem filling its entry
    *fs = filesystem;
}

void fs_load(){
    fs_static_load();
}

void fs_init(){
    memset(filesystems, 0, sizeof(filesystems));
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

struct filesystem* fs_resolve(struct disk* disk){
    struct filesystem* fs = 0;
    for(int i=0; i<BOIOS_MAX_FILESYSTEMS; i++){
        if(filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0){
            fs = filesystems[i];
            break;
        }
    }
    return fs;
}

int fopen(const char* filename, const char* mode){
    return -EIO;
}