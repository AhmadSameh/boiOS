#include "boios.h"

int main(int argc, char** argv){
    print("Hello Friend\n");
    while(1){
        if(getkey() != 0)
            print("key is pressed");
    }
    return 0;
}