#ifndef PROCESSOR_H_
#define PROCESSOR_H_
//замена свитча на индексирование
//из массива с командами выбирать структуру с командами и из выбранной структуры доставать указатель на функцию
//написать верификатор этого массива
#include "stack.h"
#include "stdbool.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "proc_error_types.h"

#include "general_const_and_func.h"

const int kStartingProcessorCapacity = 100;
const int kRAMCapacity = 400;
const int kSquareSideLength = 20;
const int kCellsPerLine = 20;

typedef struct {
    Stack  stack;        //для данных
    Stack  return_stack; //для адресов функций

    int    instruction_counter;
    int    registers[kNRegisters];
    int*   ptr_RAM;

    int*   code_buffer;
    size_t code_buffer_size;
} Processor;

#define BODY_JUMP_FUNC_GENERATION_WITH_RETURNING_PROC_ERROR_TYPE(name, compare_sign)                    \
ProcessorErrorType ProcessOp##name(Processor* processor_pointer, int argument,                          \
                                   bool* should_increment_instruction_pointer)                          \
    {                                                                                                   \
        ElementType b = StackPOP(&processor_pointer->stack);                                            \
        ElementType a = StackPOP(&processor_pointer->stack);                                            \
                                                                                                        \
        if (a compare_sign b)                                                                           \
        {                                                                                               \
            if (argument < 0 || argument >= processor_pointer->code_buffer_size || argument % 2 != 0)   \
            {                                                                                           \
                return PROC_ERROR_INVALID_JUMP;                                                         \
            }                                                                                           \
            processor_pointer->instruction_counter = argument;                                          \
            *should_increment_instruction_pointer = false;                                              \
        }                                                                                               \
        return PROC_ERROR_NO;                                                                           \
    }

#define PROCESS_NO_ARG_STACK_OP(OP_NAME)                         \
    case OP_##OP_NAME:                                           \
    {                                                            \
        stack_error = Stack##OP_NAME(&processor_pointer->stack); \
        break;                                                   \
    }

#define PROCESS_WITH_ARG_STACK_OP(OP_NAME, ARG)                         \
    case OP_##OP_NAME:                                                  \
    {                                                                   \
        stack_error = Stack##OP_NAME(&processor_pointer->stack, ARG);   \
        break;                                                          \
    }

#define PROCESS_JUMP_OP(OP_NAME)                                                                             \
    case OP_##OP_NAME:                                                                                       \
    {                                                                                                        \
        if (processor_pointer->stack.size < 2)                                                               \
        {                                                                                                    \
            proc_error = PROC_ERROR_STACK_OPERATION_FAILED;                                                  \
            break;                                                                                           \
        }                                                                                                    \
        proc_error = ProcessOp##OP_NAME(processor_pointer, argument, &should_increment_instruction_pointer); \
        break;                                                                                               \
    }

const char* GetProcErrorString(ProcessorErrorType error);
ProcessorErrorType ReadBinaryFileToBuffer(Processor* processor_pointer, const char* binary_filename);

ProcessorErrorType ExecuteProcessor(Processor* processor_pointer);

ProcessorErrorType ProcessorCtor(Processor* processor_pointer, size_t starting_capacity, const char* binary_filename);
void ProcessorDtor(Processor* processor_pointer);
void ProcDump(const Processor* proc, int errors, const char* msg);

ProcessorErrorType ProcessOpJB (Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer);
ProcessorErrorType ProcessOpJBE(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer);
ProcessorErrorType ProcessOpJA (Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer);
ProcessorErrorType ProcessOpJAE(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer);
ProcessorErrorType ProcessOpJE (Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer);
ProcessorErrorType ProcessOpJNE(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer);

#endif // PROCESSOR_H_
