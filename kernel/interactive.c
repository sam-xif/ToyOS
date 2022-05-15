#include "interactive.h"
#include "kmain.h"

static int strlen(char *str)
{
    int len = 0;
    while (str) {
        len++;
        str++;
    }
    return len;
}

int execute(char *str)
{
    // convert hex strings into bytes
    // move bytes into buffer
    // call assembly function which calls thunk
    //
    // user who inputted the code is responsible for making sure it returns properly 

    int num_bytes = strlen(str) / 2;
    byte machine_code[num_bytes];
    
    int i;
    for (i = 0; i <= num_bytes * 2; i += 2) {
        byte a = hex_to_nibble(str[i]);
        a <<= 4;
        a |= hex_to_nibble(str[i + 1]);
        machine_code[i / 2] = a;
    }

    
}

