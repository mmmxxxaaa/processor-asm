#include "processor.h"
//FIXME
// массив структур с командами
// добавить в каждую указатель на функцию, которая будет вып в процессоре
// при этом надо чтобы структура с командой имеющая номер n лежала бы в массиве на номере n
// тогда возможно заменить свитч индексированием к этому массиву

// в структуре, описывающей команду, должно быть имя строкой, номер как енам, указатель на функцию соответствующей команде
//#ERROR макрос
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "stack.h"
#include "operations.h"
#include "proc_error_types.h"

#if defined(PROC)
#define CALLBACK(callb) callb
#else
#define CALLBACK(callb) NULL


const ProcessorCommand processor_commands[] = { //Если в хедере, то он не видит указатели на функции (или мне просто объявление этого массива поставить после прототипов функций?)
    {"HLT",   OP_HLT,   ProcessOpHLT}, //FIXME см //ХуЙнЯ
    {"PUSH",  OP_PUSH,  ProcessOpPUSH},
    {"POP",   OP_POP,   ProcessOpPOP},
    {"ADD",   OP_ADD,   ProcessOpADD},
    {"SUB",   OP_SUB,   ProcessOpSUB},
    {"MUL",   OP_MUL,   ProcessOpMUL},
    {"DIV",   OP_DIV,   ProcessOpDIV},
    {"SQRT",  OP_SQRT,  ProcessOpSQRT},
    {"OUT",   OP_OUT,   ProcessOpOUT},
    {"IN",    OP_IN,    ProcessOpIN},

    {"JMP",   OP_JMP,   ProcessOpJMP},
    {"JB",    OP_JB,    ProcessOpJB},
    {"JBE",   OP_JBE,   ProcessOpJBE},
    {"JA",    OP_JA,    ProcessOpJA},
    {"JAE",   OP_JAE,   ProcessOpJAE},
    {"JE",    OP_JE,    ProcessOpJE},
    {"JNE",   OP_JNE,   ProcessOpJNE},

    {"CALL",  OP_CALL,  ProcessOpCALL},
    {"RET",   OP_RET,   ProcessOpRET},

    {"PUSHM", OP_PUSHM, ProcessOpPUSHM},
    {"POPM",  OP_POPM,  ProcessOpPOPM},
    {"DRAW",  OP_DRAW,  ProcessOpDRAW},
    {"PUSHR", OP_PUSHR, ProcessOpPUSHR},
    {"POPR",  OP_POPR,  ProcessOpPOPR}
};
const size_t kProcessorCommandsCount = sizeof(processor_commands) / sizeof(processor_commands[0]); //FIXME куда это-то нахуй

const char* GetProcErrorString(ProcessorErrorType error)
{
    switch (error)
    {
        case PROC_ERROR_NO:                      return "No error";
        case PROC_ERROR_ALLOCATION_FAILED:       return "Memory allocation failed";
        case PROC_ERROR_CANNOT_OPEN_BINARY_FILE: return "Cannot open binary file";
        case PROC_ERROR_READING_FILE:            return "Error reading file";
        case PROC_ERROR_UNKNOWN_OPCODE:          return "Unknown opcode";
        case PROC_ERROR_STACK_OPERATION_FAILED:  return "Stack operation failed";
        case PROC_ERROR_INVALID_STATE:           return "Invalid processor state";
        case PROC_ERROR_INVALID_REGISTER:        return "Invalid register";
        case PROC_ERROR_INVALID_JUMP:            return "Invalid jump";
        case PROC_ERROR_RAM_ACCESS:              return "Invalid access to RAM";
        default:                                 return "Unknown error";
    }
}

bool VerifyProcessorCommands()
{
    bool all_is_correct = true;

    for (size_t i = 0; i < kProcessorCommandsCount; i++)
    {
        const ProcessorCommand* cmd = &processor_commands[i];

        if (cmd->opcode != (OpCodes) i)
        {
            printf("ERROR: Command at index %lu has opcode %d (expected %lu)\n", i, cmd->opcode, i);
            all_is_correct = false;
        }

        if (cmd->name == NULL)
        {
            printf("ERROR: Command at index %lu has NULL name\n", i);
            all_is_correct = false;
        }

        if (cmd->handler == NULL)
        {
            printf("ERROR: Command at index %lu has NULL handler\n", i);
            all_is_correct = false;
        }
    }

    size_t n_of_expected_commands = OP_POPR + 1; //+1, т.к. енамчик с нуля
    if (kProcessorCommandsCount != n_of_expected_commands)
    {
        printf("ERROR: Expected %lu commands, but array has %lu\n", n_of_expected_commands, kProcessorCommandsCount);
        all_is_correct = false;
    }

    return all_is_correct;
}

ProcessorErrorType ExecuteProcessor(Processor* processor_pointer)
{
    assert(processor_pointer);
    assert(processor_pointer->code_buffer);

    if (processor_pointer->code_buffer_size % 2 != 0) //у каждой инструкции должен быть аргумент
        return PROC_ERROR_INVALID_STATE;

    int op_code = OP_ERR;
    int argument = 0;
    ProcessorErrorType proc_error = PROC_ERROR_NO;

    while (processor_pointer->instruction_counter < processor_pointer->code_buffer_size) //сюда
    {
        int current_instruction_counter = processor_pointer->instruction_counter;
        op_code  = processor_pointer->code_buffer[current_instruction_counter];
        argument = processor_pointer->code_buffer[current_instruction_counter + 1];

        bool should_increment_instruction_pointer = true;

        if (op_code < 0 || op_code >= (int)kProcessorCommandsCount)
            return PROC_ERROR_UNKNOWN_OPCODE;

        if (op_code == OP_HLT) //FIXME хуйня какая-то
            break;

        ProcessorCommand command = processor_commands[op_code];

        proc_error = command.handler(processor_pointer, argument, &should_increment_instruction_pointer);

        if (proc_error != PROC_ERROR_NO)
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

    processor_pointer->code_buffer_size = GetFileSize(binary_file);
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
        FREE_AND_NULL(processor_pointer->code_buffer);
        return PROC_ERROR_READING_FILE;
    }

    processor_pointer->code_buffer_size = total_ints;
    return PROC_ERROR_NO;
}

ProcessorErrorType ProcessorCtor(Processor* processor_pointer, size_t starting_capacity, const char* binary_filename)
{
    assert(processor_pointer);
    assert(starting_capacity > 0);

    if (!VerifyProcessorCommands()) //FIXME он должен быть тут или в начале функции EXECUTEPROCESSOR со статик флагом, что мы уже проверили
        return PROC_ERROR_INVALID_STATE;

    int stack_ctor_result = StackCtor(&(processor_pointer->stack), starting_capacity);
    if (stack_ctor_result != ERROR_NO)
        return PROC_ERROR_STACK_OPERATION_FAILED;

    stack_ctor_result = StackCtor(&(processor_pointer->return_stack), starting_capacity);
    if (stack_ctor_result != ERROR_NO)
        return PROC_ERROR_STACK_OPERATION_FAILED;

    for (int i = 0; i < kNRegisters; i++)
        processor_pointer->registers[i] = 0;

    processor_pointer->instruction_counter = 0;

    processor_pointer->ptr_RAM = (int*) calloc(kRAMCapacity, sizeof(int));
    if (processor_pointer->ptr_RAM == NULL)
        return PROC_ERROR_ALLOCATION_FAILED;

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

    if (processor_pointer->ptr_RAM)
        FREE_AND_NULL(processor_pointer->ptr_RAM);

    if (processor_pointer->code_buffer)
        FREE_AND_NULL(processor_pointer->code_buffer);

    StackDtor(&(processor_pointer->stack));
    StackDtor(&(processor_pointer->return_stack));

    for (int i = 0; i < kNRegisters; i++)
        processor_pointer->registers[i] = 0;

    processor_pointer->instruction_counter = 0;
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

    StackDump(&proc->stack,        errors, "Stack in Processor");
    StackDump(&proc->return_stack, errors, "Return Stack in Processor");

    const char* register_names[] = {"RAX", "RBX", "RCX", "RDX", "REX", "RFX", "RGX", "RHX"};
    for (int i = 0; i < kNRegisters; i++)
        printf("%s: %d\n", register_names[i], proc->registers[i]);

    printf("Pointer to RAM:      %p\n", proc->ptr_RAM);
    if (proc->ptr_RAM == NULL)
        printf("RAM is was not allocated\n");
    else
    {
        printf("RAM has %d cells\n", kRAMCapacity);

        for (int i = 0; i < kRAMCapacity; i++)
        {
            printf("%4d", proc->ptr_RAM[i]);

            if ((i+1) % kCellsPerLine == 0)
                printf("\n");
            else
                printf(" ");
        }
        if (kRAMCapacity % kCellsPerLine != 0) //Если последняя строка неполная
            printf("\n");
    }

    printf("Instruction counter: %d\n",  proc->instruction_counter);
    printf("Code buffer size:    %lu\n", proc->code_buffer_size);
    printf("Code buffer address: %p\n",  proc->code_buffer);
}

ProcessorErrorType ProcessOpHLT(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)//ХУЙНЯ ОНА НАХУЙ НЕ НУЖНА, НО ПРИ ЭТОМ ЧТО Я ТОГДА ЗАСУНУ В МАССИВ СТРУКТУР???
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    return PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpPUSH(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    int stack_error = StackPUSH(&processor_pointer->stack, argument);
    return (stack_error != ERROR_NO) ? PROC_ERROR_STACK_OPERATION_FAILED : PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpPOP(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    int stack_error = StackPOP(&processor_pointer->stack);
    return (stack_error != ERROR_NO) ? PROC_ERROR_STACK_OPERATION_FAILED : PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpADD(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    int stack_error = StackADD(&processor_pointer->stack);
    return (stack_error != ERROR_NO) ? PROC_ERROR_STACK_OPERATION_FAILED : PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpSUB(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    int stack_error = StackSUB(&processor_pointer->stack);
    return (stack_error != ERROR_NO) ? PROC_ERROR_STACK_OPERATION_FAILED : PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpMUL(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    int stack_error = StackMUL(&processor_pointer->stack);
    return (stack_error != ERROR_NO) ? PROC_ERROR_STACK_OPERATION_FAILED : PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpDIV(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    int stack_error = StackDIV(&processor_pointer->stack);
    return (stack_error != ERROR_NO) ? PROC_ERROR_STACK_OPERATION_FAILED : PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpSQRT(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    int stack_error = StackSQRT(&processor_pointer->stack);
    return (stack_error != ERROR_NO) ? PROC_ERROR_STACK_OPERATION_FAILED : PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpOUT(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    if (processor_pointer->stack.size == 0)
        return PROC_ERROR_STACK_OPERATION_FAILED;

    ElementType value = StackPOP(&processor_pointer->stack);
    printf("OUT -> %d\n", value);

    return PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpIN(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    int value = 0;

    printf("Enter a number: ");
    scanf("%d", &value);
    int stack_error = StackPUSH(&processor_pointer->stack, value);
    if (stack_error != ERROR_NO)
        return PROC_ERROR_STACK_OPERATION_FAILED;

    return PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpCALL(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    if (argument < 0 || argument >= processor_pointer->code_buffer_size || argument % 2 != 0)
        return PROC_ERROR_INVALID_JUMP;

    int return_address = processor_pointer->instruction_counter + 2;
    int stack_error = StackPUSH(&processor_pointer->return_stack, return_address);
    if (stack_error != ERROR_NO)
        return PROC_ERROR_STACK_OPERATION_FAILED;

    processor_pointer->instruction_counter = argument;
    *should_increment_instruction_pointer = false;

    return PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpRET(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    if (processor_pointer->return_stack.size == 0)
        return PROC_ERROR_STACK_OPERATION_FAILED;

    int return_address = StackPOP(&processor_pointer->return_stack);

    if (return_address < 0 || return_address >= processor_pointer->code_buffer_size || return_address % 2 != 0)
        return PROC_ERROR_INVALID_JUMP;

    processor_pointer->instruction_counter = return_address;
    *should_increment_instruction_pointer = false;

    return PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpPUSHM(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    if (processor_pointer->ptr_RAM == NULL)
        return PROC_ERROR_RAM_ACCESS;

    if (argument < 0 || argument >= kNRegisters)
        return PROC_ERROR_RAM_ACCESS;

    int address = processor_pointer->registers[argument];
    if (address < 0 || address >= kRAMCapacity)
        return PROC_ERROR_RAM_ACCESS;

    ElementType value = processor_pointer->ptr_RAM[address];
    StackPUSH(&processor_pointer->stack, value);

    return PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpPOPM(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    if (processor_pointer->ptr_RAM == NULL)
        return PROC_ERROR_RAM_ACCESS;

    if (argument < 0 || argument >= kNRegisters)
        return PROC_ERROR_RAM_ACCESS;

    int address = processor_pointer->registers[argument];
    if (address < 0 || address >= kRAMCapacity)
        return PROC_ERROR_RAM_ACCESS;

    if (processor_pointer->stack.size == 0)
        return PROC_ERROR_STACK_OPERATION_FAILED;

    ElementType value = StackPOP(&processor_pointer->stack);
    processor_pointer->ptr_RAM[address] = value;

    return PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpDRAW(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    if (processor_pointer->ptr_RAM == NULL)
        return PROC_ERROR_RAM_ACCESS;

    for (int i = 0; i < kRAMCapacity; i++)
    {
        printf("%c", processor_pointer->ptr_RAM[i] ? '#' : '.');
        printf(" ");
        if ((i + 1) % kSquareSideLength == 0)
            printf("\n");
    }
    printf("\n");

    return PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpPUSHR(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    if (argument < 0 || argument >= kNRegisters)
        return PROC_ERROR_INVALID_REGISTER;

    ElementType dusha_registra = processor_pointer->registers[argument];
    int stack_error = StackPUSH(&processor_pointer->stack, dusha_registra);
    if (stack_error != ERROR_NO)
        return PROC_ERROR_STACK_OPERATION_FAILED;

    return PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpPOPR(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    if (argument < 0 || argument >= kNRegisters)
        return PROC_ERROR_INVALID_REGISTER;

    if (processor_pointer->stack.size == 0)
        return PROC_ERROR_STACK_OPERATION_FAILED;

    ElementType value = StackPOP(&processor_pointer->stack);
    processor_pointer->registers[argument] = value;

    return PROC_ERROR_NO;
}

ProcessorErrorType ProcessOpJMP(Processor* processor_pointer, int argument, bool* should_increment_instruction_pointer)
{
    assert(processor_pointer);
    assert(should_increment_instruction_pointer);

    if (argument < 0 || argument >= processor_pointer->code_buffer_size || argument % 2 != 0) //%2, чтобы указатель указывал на операцию, а не на её аргумент (операции нумеруются с 0)
        return PROC_ERROR_INVALID_JUMP;

    processor_pointer->instruction_counter = argument;
    *should_increment_instruction_pointer = false;

    return PROC_ERROR_NO;
}

BODY_JUMP_FUNC_GENERATION_WITH_RETURNING_PROC_ERROR_TYPE(JB,  <)
BODY_JUMP_FUNC_GENERATION_WITH_RETURNING_PROC_ERROR_TYPE(JBE, <=)
BODY_JUMP_FUNC_GENERATION_WITH_RETURNING_PROC_ERROR_TYPE(JA,  >)
BODY_JUMP_FUNC_GENERATION_WITH_RETURNING_PROC_ERROR_TYPE(JAE, >=)
BODY_JUMP_FUNC_GENERATION_WITH_RETURNING_PROC_ERROR_TYPE(JE,  ==)
BODY_JUMP_FUNC_GENERATION_WITH_RETURNING_PROC_ERROR_TYPE(JNE, !=)

