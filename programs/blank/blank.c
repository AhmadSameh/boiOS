#include "boios.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <stdbool.h>


int main(int argc, char** argv){
    char str[] = "Hello Friend";
    struct command_argument* root_command = boios_parse_command(str, sizeof(str));
    printf("%s\n", root_command->argument);
    printf("%s\n", root_command->next->argument);
    while(1){
    }
    return 0;
}