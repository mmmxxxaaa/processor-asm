#include <stdio.h>
#include <stdlib.h>

#include "stack_error_types.h"
#include "stack.h"
#include "processor.h"

int main()
{
    Processor proc_struct = {};
    ProcessorErrorType error_result = ProcessorCtor(&proc_struct, kStartingProcessorCapacity);
    if (error_result != PROC_ERROR_NO)
    {
        fprintf(stderr, "Execution binary file failed with error %s\n", GetProcErrorString(error_result));
        ProcessorDtor(&proc_struct);
        return 1;
    }

    error_result = ExecuteBinary("../my_binary_file.txt", &proc_struct); //FIXME через аргументы командной строки
    if (error_result != PROC_ERROR_NO)
    {
        fprintf(stderr, "Execution binary file failed with error %s\n", GetProcErrorString(error_result));
        ProcessorDtor(&proc_struct);
        return 1;
    }

    ProcessorDtor(&proc_struct);
    return 0;
}
