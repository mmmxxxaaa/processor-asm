#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "my_assembler.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//содержит стек, массив с регистрами, счётчик инструкций, массив с кодом
typedef struct {
    Stack stack;
    int registers[8];
    int instruction_counter;
    int code_buffer[10]; //FIXME какая-то дичь
} Processor;

int ExecuteBinary(const char* filename);

#endif // PROCESSOR_H_
