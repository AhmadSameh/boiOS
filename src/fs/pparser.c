#include "pparser.h"

static int pathparser_path_valid_format(const char *filename){
    int len = strnlen(filename, BOIOS_MAX_PATH);
    // a valid path has to be 3 bytes long and 2nd and third bytes are  ":/" eg. 0:/
    return (len >=3 && isdigit(filename[0])) && (memcmp((void*)&filename[1], ":/", 2) == 0);  
}

static int pathparser_get_drive_by_path(const char** path){
    if(!pathparser_path_valid_format(*path))
        return -EBADPATH;
    // return the drive number from path eg. 0 from 0:/
    int drive_no = tonumericdigit(*path[0]);
    // add 3 bytes to skip drive number to skip the drive number and the ":/"
    *path += 3;
    return drive_no;
}

// initialize the a root with next entry set as 0
static struct path_root* pathparser_create_root(int drive_no){
    struct path_root* path_r = kzalloc(sizeof(struct path_root));
    path_r->drive_no = drive_no;
    path_r->first = 0;
    return path_r;
}

static const char* pathparser_get_path_part(const char** path){
    char* result_path_part = kzalloc(BOIOS_MAX_PATH);
    int i = 0;
    // select path name from path until delimeter which is "/"
    while(**path != '/' && **path != 0){
        result_path_part[i] = **path;
        *path += 1;
        i++;
    }
    // skip forward slash so loop does not immediately terminate at next loop
    if(**path == '/')
        *path += 1;
    // if no path was found, free the allocated memory and return null
    if(i == 0){
        kfree(result_path_part);
        result_path_part = 0;
    }
    return result_path_part;
}

struct path_part* pathparser_parse_path_part(struct path_part* last_part, const char** path){
    // get the name of the current path part
    const char* path_part_str = pathparser_get_path_part(path);
    if(!path_part_str)
        return 0;
    // allocate memory for the current path part and set the next part to null
    struct path_part* part = kzalloc(sizeof(struct path_part));
    part->part_name = path_part_str;
    part->next = 0;
    // if the previous part was the last one, set its next pointer to the current path
    if(last_part)
        last_part->next = part;
    return part;
}

void pathparser_free(struct path_root* root){
    struct path_part* part = root->first;
    // free each part after root
    while(part){
        struct path_part* next_part = part->next;
        kfree((void*)part->part_name);
        kfree(part);
        part = next_part;
    }
    // free the root
    kfree(root);
}

struct path_root* pathparser_parse(const char* path, const char* current_directory_path){
    int drive_no = 0;
    // tmp path so pointer of orignal path does not change
    const char* tmp_path = path; 
    // initialize root to null
    struct path_root* path_root = 0;
    if(strlen(path) > BOIOS_MAX_PATH)
        goto out;
    // get drive number from given path
    drive_no = pathparser_get_drive_by_path(&tmp_path);
    if(drive_no < 0)
        goto out;
    // create root from the drive number 
    path_root = pathparser_create_root(drive_no);
    if(!path_root)
        goto out;
    // get first part from path then add it to the path root
    struct path_part* first_part = pathparser_parse_path_part(NULL, &tmp_path);
    if(!first_part)
        goto out;
    path_root->first = first_part;
    // parse the rest of the path parts to the first part of the path
    struct path_part* part = pathparser_parse_path_part(first_part, &tmp_path);
    while(part)
        part = pathparser_parse_path_part(part, &tmp_path);
out:
    return path_root;
}
