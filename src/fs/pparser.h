#ifndef PPARSER_H
#define PPARSER_H

#include "../kernel.h"
#include "../status.h"
#include "../memory/memory.h"
#include "../string/string.h"

struct path_root{
    int drive_no;
    struct path_part* first;
};

struct path_part{
    const char* part_name;
    struct path_part* next;
};

struct path_root* pathparser_parse(const char* path, const char* current_directory_path);
void pathparser_free(struct path_root* root);

#endif