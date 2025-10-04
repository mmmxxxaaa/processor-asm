#include "processor.h"

#include <assert.h>

#include "stack.h"
#include "operations.h"
#include "my_assembler.h"

static const int kStartingCapacity = 10;

// FIXME сделай енам для ошибок / состояний процессора
int ExecuteBinary(const char* binary_filename)
{
    assert(binary_filename != NULL);

// Аналогично ассемблеру - нужна структура с массивом кода, pc (program counter)
// Конструктор нужен и деструктор

    FILE* binary_file = fopen(binary_filename, "rb");
    if (binary_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open binary file %s\n", binary_filename); // strerror или perror
        return -1;
    }

    INIT(stack);
    int stack_init_result  = StackCtor(&stack, kStartingCapacity);
    if (stack_init_result  != ERROR_NO)
    {
        fprintf(stderr, "Error: Failed to initialize stack\n");
        fclose(binary_file);
        return -1;
    }

    int op_code = OP_ERR;
    int argument = 0;

    while (fread(&op_code, sizeof(int), 1, binary_file) == 1 &&   //FIXME ОЧЕНЬ ДОРОГО, нужно разом всё читать и хранить в буфере
           fread(&argument, sizeof(int), 1, binary_file) == 1)
    {
        switch (op_code)
        {
            case OP_PUSH:
                if (StackPush(&stack, argument) != ERROR_NO)
                {
                    fprintf(stderr, "Error: StackPush failed\n");
                    fclose(binary_file);
                    StackDtor(&stack);
                    return -1;
                }
                break;

            case OP_POP:
            {
                ElementType popped = StackPop(&stack);
                if (popped == kPoison)
                {
                    fprintf(stderr, "Error: StackPop failed\n");
                    fclose(binary_file);
                    StackDtor(&stack);
                    return -1;
                }
                break;
            }

            case OP_ADD:
            {
                int errors = StackAdd(&stack);
                if (errors != 0)
                {
                    StackDump(&stack, errors, "StackAdd failed in ADD\n");
                    fclose(binary_file);
                    StackDtor(&stack);
                    return -1;
                }
                break;
            }

            case OP_SUB:
            {
                int errors = StackSub(&stack);
                if (errors != 0)
                {
                    StackDump(&stack, errors, "StackSub failed in SUB\n");
                    fclose(binary_file);
                    StackDtor(&stack);
                    return -1;
                }
                break;
            }

            case OP_MUL:
            {
                int errors = StackMul(&stack);
                if (errors != 0)
                {
                    StackDump(&stack, errors, "StackMul failed in MUL\n");
                    fclose(binary_file);
                    StackDtor(&stack);
                    return -1;
                }
                break;
            }

            case OP_DIV:
            {
                int errors = StackDiv(&stack);
                if (errors != 0)
                {
                    StackDump(&stack, errors, "StackDiv failed in DIV\n");
                    fclose(binary_file);
                    StackDtor(&stack);
                    return -1;
                }
                break;
            }

            case OP_SQRT:
            {
                int errors = StackSqrt(&stack);
                if (errors != 0)
                {
                    StackDump(&stack, errors, "StackSqrt failed in SQRT\n");
                    fclose(binary_file);
                    StackDtor(&stack);
                    return -1;
                }
                break;
            }

            case OP_OUT:
            {
                if (stack.size == 0)
                {
                    fprintf(stderr, "Error: Stack empty for OUT\n");
                    fclose(binary_file);
                    StackDtor(&stack);
                    return -1;
                }

                ElementType value = StackPop(&stack);
                if (value == kPoison)
                {
                    fprintf(stderr, "Error: StackPop failed in OUT\n");
                    fclose(binary_file);
                    StackDtor(&stack);
                    return -1;
                }
                printf("OUT -> %d\n", value);
                break;
            }

            case OP_HLT:
                fclose(binary_file);
                StackDtor(&stack);
                return 0;

            default:
                fprintf(stderr, "Error: Unknown opcode %d\n", op_code);
                fclose(binary_file);
                StackDtor(&stack);
                return -1;
        }

    }

//деструктор для структуры проца сделай
    fclose(binary_file);
    StackDtor(&stack);
    return 0;
}



int ProcessorCtor(Processor* processor_pointer, size_t starting_capacity)
{
    assert(processor_pointer);
    INIT(processor_stack); //FIXME что тут делать
    processor_pointer -> stack = processor_stack;
    StackCtor(&(processor_pointer -> stack), starting_capacity);
    for (int i = 0; i < 8; i++)
    {
        processor_pointer->registers[i] = 0;
    }
    processor_pointer -> instruction_counter = 0;
    processor_pointer -> code_buffer       = NULL;
    //FIXME добавить enum ошибок проца и написать ProcessorDump
    // if (processor_pointer -> code_buffer == NULL)
    // {
    //     int errors = ERROR_ALLOCATION_FAILED;
    //     processorDump(processor_pointer, errors, "Allocating memory in AssembleCtor failed");
    //     return errors;
    // }

    return ERROR_NO;
}

void ProcessorDtor(Processor* processor_pointer)
{
    if (processor_pointer -> code_buffer)
        free(processor_pointer -> code_buffer);
    StackDtor(&(processor_pointer -> stack));

    for (int i = 0; i < 8; i++)
    {
        processor_pointer->registers[i] = 0;
    }
    processor_pointer -> instruction_counter = 0;
}
