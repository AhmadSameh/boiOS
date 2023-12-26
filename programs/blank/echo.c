#include "boios.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "shell.h"
#include "unistd.h"
#include <stdbool.h>


int main(int argc, char** argv){
    // for(int i=1; i<argc; i++)
    //     printf("%s ", argv[i]);
    while(1)
    {   
        sleep(1000);
        print(argv[0]);
    }
    return 0;
}