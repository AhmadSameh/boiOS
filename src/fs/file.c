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
    fs_insert_filesystem(fat16_init());
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

static void file_free_descriptor(struct file_descriptor* desc){
    file_descriptors[desc->index - 1] = 0x00;
    kfree(desc);
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

FILE_MODE file_get_mode_by_string(const char* str){
    FILE_MODE mode = FILE_MODE_INVALID;
    if(strncmp(str, "r", 1) == 0)
        mode = FILE_MODE_READ;
    else if(strncmp(str, "w", 1) == 0)
        mode = FILE_MODE_WRITE;
    else if(strncmp(str, "a", 1) == 0)
        mode = FILE_MODE_APPEND;
    return mode;
}

int fopen(const char* filename, const char* mode_str){
    int response = 0;
    // first parse the path into path root and path parts
    struct path_root* root_path = pathparser_parse(filename, NULL);
    if(!root_path){
        response = -EINVARG;
        goto out;
    }
    // this is a root path e.g. 0:/, which we can not have
    if(!root_path->first){
        response = -EINVARG;
        goto out;
    }
    // get the current disk first from file path
    struct disk* disk = disk_get(root_path->drive_no);
    // if disk is not found return error
    if(!disk){
        response = -EIO;
        goto out;
    }
    // if the disk has no filesystem, return error
    if(!disk->filesystem){
        response = -EIO;
        goto out;
    }
    // get mode of the open function, values are: read, write & append
    FILE_MODE mode = file_get_mode_by_string(mode_str);
    if(mode == FILE_MODE_INVALID){
        response = -EINVARG;
        goto out;
    }
    // call the open function located in the filesystem, it is abstracted out, now we have the private data
    void* descriptor_private_date = disk->filesystem->open(disk, root_path->first, mode);
    if(ISERR(descriptor_private_date)){
        response = ERROR_I(descriptor_private_date);
        goto out;
    }
    // create a new file descriptor
    struct file_descriptor* desc = 0;
    response = file_new_descriptor(&desc);
    if(response < 0)
        goto out;
    // set the created descriptor data to the filesystem data of the opened file
    desc->filesystem = disk->filesystem;
    desc->private = descriptor_private_date;
    desc->disk = disk;
    response = desc->index;
out:
    // must return 0 at failure not -ve
    if(response < 0)
        response = 0;
    return response;
}

int fstat(int fd, struct file_stat* stat){
    int response = 0;
    struct file_descriptor* desc = file_get_descriptor(fd);
    if(!desc){
        response = -EIO;
        goto out;
    }
    response = desc->filesystem->stat(desc->disk, desc->private, stat);
out:
    return response;
}

int fseek(int fd, int offset, FILE_SEEK_MODE whence){
    int response = 0;
    struct file_descriptor* desc = file_get_descriptor(fd);
    if(!desc){
        response = -EIO;
        goto out;
    }
    response = desc->filesystem->seek(desc->private, offset, whence);
out:
    return response;
}

int fread(void* ptr, uint32_t size, uint32_t nmemb, int fd){
    int response = 0;
    if(size == 0 || nmemb == 0 || fd < 1){
        response = -EINVARG;
        goto out;
    }
    struct file_descriptor* desc = file_get_descriptor(fd);
    if(!desc){
        response = -EINVARG;
        goto out;
    }
    response = desc->filesystem->read(desc->disk, desc->private, size, nmemb, (char*)ptr);
out:
    return response;
}

int fclose(int fd){
    int response = 0;
    struct file_descriptor* desc = file_get_descriptor(fd);
    if(!desc){
        response = -EIO;
        goto out;
    }
    response = desc->filesystem->close(desc->private);
    if(response == BOIOS_ALL_OK)
        file_free_descriptor(desc);
out:
    return response;
}