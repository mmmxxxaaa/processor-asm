#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "proc_error_types.h"

#include "my_assembler.h"

const int kStartingProcessorCapacity = 100;

typedef struct {
    Stack stack;
    int registers[kNRegisters];
    int instruction_counter;
    int* code_buffer;
    size_t code_buffer_size;
} Processor;

const char* GetProcErrorString(ProcessorErrorType error);
ProcessorErrorType ReadBinaryFileToBuffer(Processor* processor_pointer, const char* binary_filename);

ProcessorErrorType ExecuteBinary(const char* filename, Processor* proc_struct_pointer);
ProcessorErrorType ExecuteProcessor(Processor* processor_pointer);


ProcessorErrorType ProcessorCtor(Processor* processor_pointer, size_t starting_capacity);
void ProcessorDtor(Processor* processor_pointer);
void ProcDump(const Processor* proc, int errors, const char* msg);

long int GetSizeOfBinaryFile(FILE* binary_file);

ProcessorErrorType ProcessOpJB (Processor* processor_pointer, int argument, int* should_increment_instruction_pointer); //FIXME возвращать тип процессорной ошибки
ProcessorErrorType ProcessOpJBE(Processor* processor_pointer, int argument, int* should_increment_instruction_pointer); //FIXME возвращать тип процессорной ошибки
ProcessorErrorType ProcessOpJA (Processor* processor_pointer, int argument, int* should_increment_instruction_pointer); //FIXME возвращать тип процессорной ошибки
ProcessorErrorType ProcessOpJAE(Processor* processor_pointer, int argument, int* should_increment_instruction_pointer); //FIXME возвращать тип процессорной ошибки
ProcessorErrorType ProcessOpJE (Processor* processor_pointer, int argument, int* should_increment_instruction_pointer); //FIXME возвращать тип процессорной ошибки
ProcessorErrorType ProcessOpJNE(Processor* processor_pointer, int argument, int* should_increment_instruction_pointer); //FIXME возвращать тип процессорной ошибки

#endif // PROCESSOR_H_
