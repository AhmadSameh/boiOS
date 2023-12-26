#ifndef BOIOS_H
#define BOIOS_H

#include <stddef.h>
#include <stdbool.h>

struct command_argument{
    char argument[512];
    struct command_argument* next;
};

struct process_arguments{
    int argc;
    char** argv;
};

struct command_argument* boios_parse_command(const char* command, int max);
void print(const char* msg);
int boios_getkey(void);
int boios_getkeyblock(void);
void boios_terminal_readline(char* out, int max, bool output_while_typing);
void boios_putchar(char c);
void* boios_malloc(size_t size);
void boios_free(void* ptr);
void boios_process_load_start(const char* filename);
int boios_system(struct command_argument* arguments);
void boios_process_get_arguments(struct process_arguments* arguments);
int boios_system_run(const char* command);
void boios_exit(void);
int boios_fopen(const char* filename);
void boios_sleep(int millis);
void boios_clr_terminal(void);

#endif