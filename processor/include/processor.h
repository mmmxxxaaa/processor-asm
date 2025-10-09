#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "proc_error_types.h"

#include "general_const_and_func.h"

const int kStartingProcessorCapacity = 100;

typedef struct {
    Stack stack;
    int registers[kNRegisters];
    int instruction_counter;
    int* code_buffer;
    size_t code_buffer_size;
} Processor;

#define DEFINE_JUMP_FUNC_GENERATION(name, compare_sign) \
ProcessorErrorType ProcessOp##name(Processor* processor_pointer, int argument, \
                                   int* should_increment_instruction_pointer) \
{ \
    ElementType b = StackPop(&processor_pointer->stack); \
    ElementType a = StackPop(&processor_pointer->stack); \
    \
    if (a compare_sign b) \
    { \
        if (argument < 0 || argument >= processor_pointer->code_buffer_size || argument % 2 != 0) \
        { \
            return PROC_ERROR_INVALID_JUMP; \
        } \
        processor_pointer->instruction_counter = argument; \
        *should_increment_instruction_pointer = 0; \
    } \
    return PROC_ERROR_NO; \
}

const char* GetProcErrorString(ProcessorErrorType error);
ProcessorErrorType ReadBinaryFileToBuffer(Processor* processor_pointer, const char* binary_filename);

ProcessorErrorType ExecuteProcessor(Processor* processor_pointer);

ProcessorErrorType ProcessorCtor(Processor* processor_pointer, size_t starting_capacity, const char* binary_filename);
void ProcessorDtor(Processor* processor_pointer);
void ProcDump(const Processor* proc, int errors, const char* msg);

long int GetSizeOfBinaryFile(FILE* binary_file);
// char* AddPrefixWithStrcat(const char* input_filename, const char* prefix);

ProcessorErrorType ProcessOpJB (Processor* processor_pointer, int argument, int* should_increment_instruction_pointer);
ProcessorErrorType ProcessOpJBE(Processor* processor_pointer, int argument, int* should_increment_instruction_pointer);
ProcessorErrorType ProcessOpJA (Processor* processor_pointer, int argument, int* should_increment_instruction_pointer);
ProcessorErrorType ProcessOpJAE(Processor* processor_pointer, int argument, int* should_increment_instruction_pointer);
ProcessorErrorType ProcessOpJE (Processor* processor_pointer, int argument, int* should_increment_instruction_pointer);
ProcessorErrorType ProcessOpJNE(Processor* processor_pointer, int argument, int* should_increment_instruction_pointer);

#endif // PROCESSOR_H_
