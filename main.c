#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern int process1(int* argc, char** argv[]);

int main(int argc, char* argv[]) {
    return process1(&argc, &argv);
}
