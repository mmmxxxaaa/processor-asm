#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "proc_error_types.h"

#include "my_assembler.h"

typedef struct {
    Stack stack;
    int registers[kNRegisters];
    int instruction_counter;
    int* code_buffer;
    size_t code_buffer_size;
} Processor;

const char* GetProcErrorString(ProcessorErrorType error);
ProcessorErrorType ReadBinaryFileToBuffer(Processor* processor_pointer, const char* binary_filename);
ProcessorErrorType ExecuteProcessor(Processor* processor_pointer);


ProcessorErrorType ProcessorCtor(Processor* processor_pointer, size_t starting_capacity);
void ProcessorDtor(Processor* processor_pointer);
void ProcDump(const Processor* proc, int errors, const char* msg);

long int GetSizeOfBinaryFile(FILE* binary_file);
ProcessorErrorType ExecuteBinary(const char* filename);

#endif // PROCESSOR_H_
