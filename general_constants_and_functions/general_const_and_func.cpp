#include "general_const_and_func.h"

#include <stdlib.h>
#include <stdio.h>


long int GetFileSize(FILE* file)
{
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return file_size;
}
