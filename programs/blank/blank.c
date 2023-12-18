#include "boios.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <stdbool.h>


int main(int argc, char** argv){
    struct process_arguments arguments;
    boios_process_get_arguments(&arguments);
    printf("%i %s\n", arguments.argc, arguments.argv[0]);
    while(1){
    }
    return 0;
}