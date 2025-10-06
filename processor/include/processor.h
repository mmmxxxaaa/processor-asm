#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h> //FIXME разобраться с инклюдами
#include "proc_error_types.h"

// #if defined(DEBUG_CANARY) || defined(DEBUG_HASH)
//     #if defined(DEBUG_HASH)
//         // Если определен DEBUG_HASH (с хэшем)
//         #define INITPROC(name) Processor name = {{{__func__, __LINE__, __FILE__, #name, 0}, NULL, 0, 0}, {0}, 0, NULL}
//     #else
//         // Если определен только DEBUG_CANARY (без хэша)
//         #define INITPROC(name) Processor name = {{{__func__, __LINE__, __FILE__, #name}, NULL, 0, 0}, {0}, 0, NULL}
//     #endif
// #else
//     // Если ни один отладочный макрос не определен
//     #define INITPROC(name) Processor name = {{NULL, 0, 0}, {0}, 0, NULL}
// #endif // defined(DEBUG_CANARY) || defined(DEBUG_HASH)

const int kNRegisters = 8;

typedef struct {
    Stack stack;
    int registers[kNRegisters];
    int instruction_counter;
    int* code_buffer;
    size_t code_buffer_size;
} Processor;
//FIXME написать конструктор и деструктор этой структуры проца

const char* GetProcErrorString(ProcessorErrorType error);
ProcessorErrorType ReadBinaryFileToBuffer(Processor* processor_pointer, const char* binary_filename);
ProcessorErrorType ExecuteProcessor(Processor* processor_pointer);


ProcessorErrorType ProcessorCtor(Processor* processor_pointer, size_t starting_capacity);
void ProcessorDtor(Processor* processor_pointer);
void ProcDump(const Processor* proc, int errors, const char* msg);

long int GetSizeOfBinaryFile(FILE* binary_file);
ProcessorErrorType ExecuteBinary(const char* filename);

#endif // PROCESSOR_H_
