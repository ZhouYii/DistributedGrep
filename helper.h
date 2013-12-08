#ifndef HELPER_H
#define HELPER_H
#include <stdio.h>
#include <stdlib.h>
void err_handler(const char* msg) {
    if(msg)
        printf("%s\n", msg);
    exit(1);
}
#endif
