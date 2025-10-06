#include "processor.h"

#include <assert.h>

#include "stack.h"
#include "operations.h"
#include "my_assembler.h"
#include "proc_error_types.h"

static const int kStartingCapacity = 10;
static const int kStartingProcessorCapacity = 100;

const char* GetProcErrorString(ProcessorErrorType error)
{
    switch (error) {
        case PROC_ERROR_NO: return "No error";
        case PROC_ERROR_ALLOCATION_FAILED: return "Memory allocation failed";
        case PROC_ERROR_CANNOT_OPEN_BINARY_FILE: return "Cannot open binary file";
        case PROC_ERROR_READING_FILE: return "Error reading file";
        case PROC_ERROR_UNKNOWN_OPCODE: return "Unknown opcode";
        case PROC_ERROR_STACK_OPERATION_FAILED: return "Stack operation failed";
        case PROC_ERROR_INVALID_STATE: return "Invalid processor state";
        default: return "Unknown error";
    }
}

ProcessorErrorType ExecuteBinary(const char* binary_filename)
{
    assert(binary_filename != NULL);

    Processor proc_struct = {}; //FIXME скорее всего тут не должно быть деструкторов, они должны быть в мейне
    ProcessorErrorType ctor_result = ProcessorCtor(&proc_struct, kStartingProcessorCapacity);
    if (ctor_result != PROC_ERROR_NO)
        return ctor_result;

    ProcessorErrorType error = ReadBinaryFileToBuffer(&proc_struct, binary_filename);
    if (error != PROC_ERROR_NO)
    {
        ProcessorDtor(&proc_struct); //FIXME скорее всего тут не должно быть деструкторов, они должны быть в мейне
        return error;
    }
//FIXME напихать ПроцДАмпов
    error = ExecuteProcessor(&proc_struct);
    if (error != PROC_ERROR_NO)
    {
        ProcessorDtor(&proc_struct); //FIXME скорее всего тут не должно быть деструкторов, они должны быть в мейне
        return error;
    }

    ProcessorDtor(&proc_struct);
    return PROC_ERROR_NO;
}



ProcessorErrorType ExecuteProcessor(Processor* processor_pointer)
{
    assert(processor_pointer);
    assert(processor_pointer->code_buffer); //FIXME ???

    int op_code = OP_ERR;
    int argument = 0;

    ProcessorErrorType proc_error = PROC_ERROR_NO;
    int stack_error = 0;

    int binary_file_size = processor_pointer->code_buffer_size; //FIXME binary_file_size это уже количество интов в бинарнике
    int total_ints = binary_file_size - binary_file_size % 2; // чтоб не перескочить в цикле

    int* buffer = processor_pointer->code_buffer; //MENTOR нужно ли это делать, чтобы не тратить время на бег по стрелочке

    for (int i = 0; i < total_ints; i += 2)
    {
        op_code = buffer[i];
        argument = buffer[i+1];
        switch (op_code)
        {
            case OP_PUSH:
            {
                stack_error = StackPush(&processor_pointer->stack, argument);
                break;
            }
            case OP_POP:
            {
                ElementType popped = StackPop(&processor_pointer->stack);
            // FIXME - чо за хуйня нужна ошибка нормальная, а то вдруг в стеке лежит значение буквально пойзона
                if (popped == kPoison)
                {
                    proc_error = PROC_ERROR_STACK_OPERATION_FAILED;
                }
                break;
            }

            case OP_ADD:
            {
                stack_error = StackAdd(&processor_pointer->stack);
                break;
            }

            case OP_SUB:
            {
                stack_error = StackSub(&processor_pointer->stack);
                break;
            }

            case OP_MUL:
            {
                stack_error = StackMul(&processor_pointer->stack);
                break;
            }

            case OP_DIV:
            {
                stack_error = StackDiv(&processor_pointer->stack);
                break;
            }

            case OP_SQRT:
            {
                stack_error = StackSqrt(&processor_pointer->stack);
                break;
            }

            case OP_IN:
            {
                int value = 0;
                printf("Enter a number: ");
                scanf("%d", &value);
                stack_error = StackPush(&processor_pointer->stack, value);
                break;
            }

            case OP_OUT:
            {
            // FIXME - проверку в поп
                if (processor_pointer->stack.size == 0)
                {
                    proc_error = PROC_ERROR_STACK_OPERATION_FAILED;
                    break;
                }

                ElementType value = StackPop(&processor_pointer->stack);
                if (value == kPoison) //FIXME очень залупная проверка, надо придумать, как нужно проверять (по сути у меня в попе это уже само всё обрабатывается, поэтому возвращаемое значение можно не проверять)
                {
                    proc_error = PROC_ERROR_STACK_OPERATION_FAILED;
                    break;
                }
                printf("OUT -> %d\n", value);
                break;
            }

            case OP_HLT:
                return PROC_ERROR_NO;

            default:
                return PROC_ERROR_UNKNOWN_OPCODE;
        }

        if (proc_error != PROC_ERROR_NO || stack_error != ERROR_NO) //FIXME разобраться с совместимостью ошибок, попробовать привести их в один тип
        {
            ProcDump(processor_pointer, proc_error, "Processor Execution failed");
            return proc_error;
        }

    }

    return PROC_ERROR_NO;
}


ProcessorErrorType ReadBinaryFileToBuffer(Processor* processor_pointer, const char* binary_filename)
{
    assert(processor_pointer);
    assert(binary_filename);

    FILE* binary_file = fopen(binary_filename, "rb");
    if (binary_file == NULL)
        return PROC_ERROR_CANNOT_OPEN_BINARY_FILE;

    long int binary_file_size_bytes = GetSizeOfBinaryFile(binary_file);
    if (binary_file_size_bytes <= 0)
    {
        fclose(binary_file);
        return PROC_ERROR_READING_FILE;
    }

    size_t total_ints = binary_file_size_bytes / sizeof(int);
    processor_pointer->code_buffer = (int*) calloc(total_ints, sizeof(int));
    if (processor_pointer->code_buffer == NULL)
    {
        fclose(binary_file);
        return PROC_ERROR_ALLOCATION_FAILED;
    }

    size_t elements_read = fread(processor_pointer->code_buffer, sizeof(int), total_ints, binary_file);
    fclose(binary_file);

    if (elements_read != total_ints)
    {
        free(processor_pointer->code_buffer);
        processor_pointer->code_buffer = NULL;
        return PROC_ERROR_READING_FILE;
    }

    processor_pointer->code_buffer_size = total_ints;
    return PROC_ERROR_NO;
}



ProcessorErrorType ProcessorCtor(Processor* processor_pointer, size_t starting_capacity)
{
    assert(processor_pointer);
    assert(starting_capacity > 0);

    int stack_ctor_result = StackCtor(&(processor_pointer -> stack), starting_capacity); //FIXME не int, а enum, везде поправить
    if (stack_ctor_result != ERROR_NO)
        return PROC_ERROR_STACK_OPERATION_FAILED;

    for (int i = 0; i < 8; i++) //FIXME это вроде бы не нужно
        processor_pointer->registers[i] = 0;

    processor_pointer->instruction_counter = 0;
    processor_pointer->code_buffer         = NULL;
    processor_pointer->code_buffer_size    = 0;

    return PROC_ERROR_NO;
}

void ProcessorDtor(Processor* processor_pointer)
{
    if (!processor_pointer)
        return; //FIXME ????

    if (processor_pointer->code_buffer)
        free(processor_pointer->code_buffer);

    StackDtor(&(processor_pointer -> stack));
    for (int i = 0; i < 8; i++)
    {
        processor_pointer->registers[i] = 0;
    }
    processor_pointer->instruction_counter = 0;
    processor_pointer->code_buffer         = NULL;
    processor_pointer->code_buffer_size    = 0;
}

void ProcDump(const Processor* proc, int errors, const char* msg)
{
    assert(msg != NULL);

    if (proc == NULL)
    {
        printf("Processor [%p] %s", proc, msg);
        return;
    }

    printf("Processor [%p] Dump: %s\n", proc, msg);
    StackDump(&proc->stack, errors, "Stack in Processor");

    const char* register_names[] = {"RAX", "RBX", "RCX", "RDX", "REX", "RFX", "RGX", "RHX"}; //FIXME а что, если поменяем количество регистров?
    for (int i = 0; i < kNRegisters; i++)
        printf("%s: %d\n", register_names[i], proc->registers[i]);

    printf("Instruction counter: %d\n",  proc->instruction_counter);
    printf("Code buffer size:    %lu\n", proc->code_buffer_size);
    printf("Code buffer adress   %p\n",  proc->code_buffer);
}

long int GetSizeOfBinaryFile(FILE* binary_file)
{
    if (!binary_file)
        return -1;

    fseek(binary_file, 0L, SEEK_END);
    long int size = ftell(binary_file);
    rewind(binary_file);
    return size;
}
