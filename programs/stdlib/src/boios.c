#include "boios.h"
#include "string.h"
#include "stdio.h"
#include <stdbool.h>

struct command_argument* boios_parse_command(const char* command, int max){
    struct command_argument* root_command = 0;
    char scommand[1025];
    if(max >= (int)sizeof(scommand))
        return 0;
    strncpy(scommand, command, sizeof(scommand));
    char* token = strtok(scommand, " ");
    if(!token)
        goto out;
    root_command = boios_malloc(sizeof(struct command_argument));
    if(!root_command)
        goto out;
    strncpy(root_command->argument, token, sizeof(root_command->argument));
    root_command->next = 0;
    struct command_argument* current = root_command;
    token = strtok(NULL, " ");
    while(token != 0){
        struct command_argument* new_command = boios_malloc(sizeof(struct command_argument));
        if(!new_command)
            break;
        strncpy(new_command->argument, token, sizeof(new_command->argument));
        new_command->next = 0x00;
        current->next = new_command;
        current = new_command;
        token = strtok(NULL, " ");
    }
out:
    return root_command;
}

int boios_getkeyblock(){
    int val = 0;
    do{
        val = boios_getkey();
    }while(val == 0);
    return val;
}

void boios_terminal_readline(char* out, int max, bool output_while_typing){
    int i;
    for(i=0; i<max-1; i++){
        boios_putchar(179);
        char key = boios_getkeyblock();
        // read line when pressing enter
        if(key == 13)
            break;
        if(output_while_typing){
            boios_putchar(0x08);
            if(key == 0x08 && i == 0){
                i--;
                continue;
            }
            boios_putchar(key);
        }
        // backspace
        if(key == 0x08 && i >= 1){
            out[i-1] = 0x00;
            i -= 2;
            continue;
        }
        out[i] = key;
    }
    out[i] = 0x00;
}

int boios_system_run(const char* command){
    char buf[1024];
    strncpy(buf, command, sizeof(buf));
    struct command_argument* root_command_argument = boios_parse_command(buf, sizeof(buf));
    if(!root_command_argument)
        return -1;
    return boios_system(root_command_argument);
}