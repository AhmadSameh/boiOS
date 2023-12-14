#include "shell.h"
#include "stdio.h"
#include "stdlib.h"
#include "boios.h"

int main(int argc, char** argv){
    print("boios v0.0.1\n");
    while(1){
        print("> ");
        char buf[1024];
        boios_terminal_readline(buf, sizeof(buf), true);
        print("\n");
    }
    return 0;
}