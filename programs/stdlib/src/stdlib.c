#include "stdlib.h"
#include "boios.h"

void* malloc(size_t size){
    return boios_malloc(size);
}

void free(void* ptr){
    boios_free(ptr);
}