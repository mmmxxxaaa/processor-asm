#include "general_const_and_func.h"

#include <stdlib.h>
#include <string.h>

char* AddPrefixWithStrcat(const char* input_filename, const char* prefix)
{
    if (input_filename == NULL || prefix == NULL)
        return NULL;

    size_t prefix_len = strlen(prefix);
    size_t filename_len = strlen(input_filename);
    size_t total_len = prefix_len + filename_len + 1; // +1 для '\0'

    char* result = (char*) calloc(total_len, sizeof(char));
    if (result == NULL)
        return NULL;

    strcpy(result, prefix);
    strcat(result, input_filename);

    return result;
}
