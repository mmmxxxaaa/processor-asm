#include <stdio.h>
#include <stdlib.h>

#include "stack_error_types.h"
#include "stack.h"
#include "processor.h"

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Error in %s. You should enter one filename: name of binary file. \n", argv[0]);
        return 1;
    }

    const char* binary_filename = argv[1];

    Processor proc_struct = {};
    ProcessorErrorType error_result = ProcessorCtor(&proc_struct, kStartingProcessorCapacity, binary_filename);
    if (error_result != PROC_ERROR_NO)
    {
        fprintf(stderr, "ProcessorCtor failed with error %s\n", GetProcErrorString(error_result));
        ProcessorDtor(&proc_struct);
        return 1;
    }

    error_result = ExecuteProcessor(&proc_struct);
    if (error_result != PROC_ERROR_NO)
    {
        fprintf(stderr, "Execution Processor failed with error %s\n", GetProcErrorString(error_result));
        ProcessorDtor(&proc_struct);
        return 1;
    }

    ProcessorDtor(&proc_struct);
    return 0;
}
