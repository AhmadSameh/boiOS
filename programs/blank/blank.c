#include "boios.h"
#include "stdlib.h"

int main(int argc, char** argv){
    print("Hello Friend\n");
    void* ptr = malloc(512);
    free(ptr);
    while(1){
        if(getkey() != 0)
            print("key is pressed");
    }
    return 0;
}