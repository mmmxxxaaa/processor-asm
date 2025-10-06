#include <stdio.h>
#include <stdlib.h>

#include "stack_error_types.h"
#include "stack.h"
#include "processor.h"

int main()
{
    ProcessorErrorType result = ExecuteBinary("../my_binary_file.txt"); //FIXME через аргументы командной строки

    if (result != PROC_ERROR_NO)
        fprintf(stderr, "Execution binary file failed with error %s", GetProcErrorString(result));

    return 0;
}
