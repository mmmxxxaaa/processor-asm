#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "my_assembler.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    Stack stack;
    int registers[8];
    int instruction_counter;
    int* code_buffer;
} Processor;
//FIXME написать конструктор и деструктор этой структуры проца

int ProcessorCtor(Processor* processor_pointer, size_t starting_capacity);
void ProcessorDtor(Processor* processor_pointer);
int ExecuteBinary(const char* filename);

#endif // PROCESSOR_H_
