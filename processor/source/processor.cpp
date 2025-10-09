#include "processor.h"

#include <assert.h>
#include <string.h>

#include "stack.h"
#include "operations.h"
#include "proc_error_types.h"

// #include "general_const_and_func.h"

const char* GetProcErrorString(ProcessorErrorType error)
{
    switch (error)
    {
        case PROC_ERROR_NO: return "No error";
        case PROC_ERROR_ALLOCATION_FAILED: return "Memory allocation failed";
        case PROC_ERROR_CANNOT_OPEN_BINARY_FILE: return "Cannot open binary file";
        case PROC_ERROR_READING_FILE: return "Error reading file";
        case PROC_ERROR_UNKNOWN_OPCODE: return "Unknown opcode";
        case PROC_ERROR_STACK_OPERATION_FAILED: return "Stack operation failed";
        case PROC_ERROR_INVALID_STATE: return "Invalid processor state";
        case PROC_ERROR_INVALID_REGISTER: return "Invalid register";
        case PROC_ERROR_INVALID_JUMP: return "Invalid jump";
        default: return "Unknown error";
    }
}

ProcessorErrorType ExecuteProcessor(Processor* processor_pointer) //FIXME сигнатуру + версию добавить
{                                                                 //FIXME протестить джампы
    assert(processor_pointer);
    assert(processor_pointer->code_buffer);

    if (processor_pointer->code_buffer_size % 2 != 0) //у каждой инструкции должен быть аргумент
        return PROC_ERROR_INVALID_STATE;

    int op_code = OP_ERR;
    int argument = 0;

    ProcessorErrorType proc_error = PROC_ERROR_NO;
    int stack_error = 0;

    while (processor_pointer->instruction_counter < processor_pointer->code_buffer_size)
    {
        int current_instruction_counter = processor_pointer->instruction_counter;
        op_code = processor_pointer->code_buffer[current_instruction_counter];
        argument = processor_pointer->code_buffer[current_instruction_counter + 1];

        int should_increment_instruction_pointer = 1;

        switch (op_code)
        {
            PROCESS_WITH_ARG_STACK_OP(PUSH, argument)

            PROCESS_NO_ARG_STACK_OP(POP)
            PROCESS_NO_ARG_STACK_OP(ADD)
            PROCESS_NO_ARG_STACK_OP(SUB)
            PROCESS_NO_ARG_STACK_OP(MUL)
            PROCESS_NO_ARG_STACK_OP(DIV)
            PROCESS_NO_ARG_STACK_OP(SQRT)

            case OP_IN:
            {
                int value = 0;
                printf("Enter a number: ");
                scanf("%d", &value);
                stack_error = StackPUSH(&processor_pointer->stack, value);
                break;
            }

            case OP_JMP:
            {
                if (argument < 0 || argument >= processor_pointer->code_buffer_size || argument % 2 != 0) //%2, чтобы указатель указывал на операцию, а не на её аргумент (операции нумеруются с 0)
                {
                    proc_error = PROC_ERROR_INVALID_JUMP;
                    break;
                }
                processor_pointer->instruction_counter = argument;
                should_increment_instruction_pointer = 0;

                break;
            }

            PROCESS_JUMP_OP(JB)
            PROCESS_JUMP_OP(JBE)
            PROCESS_JUMP_OP(JA)
            PROCESS_JUMP_OP(JAE)
            PROCESS_JUMP_OP(JE)
            PROCESS_JUMP_OP(JNE)

            case OP_PUSHR: //засунуть в стек из регистра
            {
                if (argument < 0 || argument >= kNRegisters)
                {
                    proc_error = PROC_ERROR_INVALID_REGISTER;
                    break;
                }
                ElementType dusha_registra = processor_pointer->registers[argument];
                stack_error = StackPUSH(&processor_pointer->stack, dusha_registra);

                break;
            }

            case OP_POPR:  //достать из стека и поместить в регистр
            {
                if (argument < 0 || argument >= kNRegisters)
                {
                    proc_error = PROC_ERROR_INVALID_REGISTER;
                    break;
                }

                if (processor_pointer->stack.size == 0)
                {
                    proc_error = PROC_ERROR_STACK_OPERATION_FAILED;
                    break;
                }

                ElementType value = StackPOP(&processor_pointer->stack);
                processor_pointer->registers[argument] = value;
                break;
            }

            case OP_OUT:
            {
                if (processor_pointer->stack.size == 0)
                {
                    proc_error = PROC_ERROR_STACK_OPERATION_FAILED;
                    break;
                }

                ElementType value = StackPOP(&processor_pointer->stack);
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

        if (should_increment_instruction_pointer)
            processor_pointer->instruction_counter += 2;
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

    processor_pointer->code_buffer_size = GetSizeOfBinaryFile(binary_file);
    if (processor_pointer->code_buffer_size <= 0)
    {
        fclose(binary_file);
        return PROC_ERROR_READING_FILE;
    }

    size_t total_ints = processor_pointer->code_buffer_size / sizeof(int);
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

ProcessorErrorType ProcessorCtor(Processor* processor_pointer, size_t starting_capacity, const char* binary_filename)
{
    assert(processor_pointer);
    assert(starting_capacity > 0);

    int stack_ctor_result = StackCtor(&(processor_pointer -> stack), starting_capacity);
    if (stack_ctor_result != ERROR_NO)
        return PROC_ERROR_STACK_OPERATION_FAILED;

    for (int i = 0; i < 8; i++)
        processor_pointer->registers[i] = 0;

    processor_pointer->instruction_counter = 0;

    processor_pointer->code_buffer         = NULL;
    processor_pointer->code_buffer_size    = 0;

    ProcessorErrorType error = ReadBinaryFileToBuffer(processor_pointer, binary_filename);
    if (error != PROC_ERROR_NO)
        return error;

    return PROC_ERROR_NO;
}

void ProcessorDtor(Processor* processor_pointer)
{
    if (!processor_pointer)
        return;

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
    printf("Code buffer address: %p\n",  proc->code_buffer);
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

BODY_JUMP_FUNC_GENERATION_WITH_RETURNING_PROC_ERROR_TYPE(JB,  <)
BODY_JUMP_FUNC_GENERATION_WITH_RETURNING_PROC_ERROR_TYPE(JBE, <=)
BODY_JUMP_FUNC_GENERATION_WITH_RETURNING_PROC_ERROR_TYPE(JA,  >)
BODY_JUMP_FUNC_GENERATION_WITH_RETURNING_PROC_ERROR_TYPE(JAE, >=)
BODY_JUMP_FUNC_GENERATION_WITH_RETURNING_PROC_ERROR_TYPE(JE,  ==)
BODY_JUMP_FUNC_GENERATION_WITH_RETURNING_PROC_ERROR_TYPE(JNE, !=)

