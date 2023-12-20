#include "boios.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <stdbool.h>


int main(int argc, char** argv){
    print(argv[0]);
    for(int i=0; i<argc; i++)
        printf("%s\n", argv[i]);
    while(1){
    }
    return 0;
}