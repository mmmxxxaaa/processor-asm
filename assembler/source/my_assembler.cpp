#include "my_assembler.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "asm_error_types.h"

#define COMPARE_COMMAND(cmd, name) if (strcmp(command, #name) == 0) return OP_##name

OpCodes GetOpCode(const char* command)
{
    assert(command != NULL);

    COMPARE_COMMAND(command, HLT);
    COMPARE_COMMAND(command, PUSH);
    COMPARE_COMMAND(command, POP);
    COMPARE_COMMAND(command, ADD);
    COMPARE_COMMAND(command, SUB);
    COMPARE_COMMAND(command, MUL);
    COMPARE_COMMAND(command, DIV);
    COMPARE_COMMAND(command, SQRT);
    COMPARE_COMMAND(command, IN);
    COMPARE_COMMAND(command, OUT);

    COMPARE_COMMAND(command, JMP);
    COMPARE_COMMAND(command, JB);
    COMPARE_COMMAND(command, JBE);
    COMPARE_COMMAND(command, JA);
    COMPARE_COMMAND(command, JAE);
    COMPARE_COMMAND(command, JE);
    COMPARE_COMMAND(command, JNE);

    COMPARE_COMMAND(command, CALL);
    COMPARE_COMMAND(command, RET);

    COMPARE_COMMAND(command, PUSHM);
    COMPARE_COMMAND(command, POPM);
    COMPARE_COMMAND(command, DRAW);

    COMPARE_COMMAND(command, PUSHR);
    COMPARE_COMMAND(command, POPR);

    fprintf(stderr, "Error: Unknown command '%s'\n", command);
    return OP_ERR;
}

const char* GetAsmErrorString(AssemblerErrorType error)
{
    switch (error) {
        case ASM_ERROR_NO:                      return "No error";
        case ASM_ERROR_UNKNOWN_COMMAND:         return "Unknown command";
        case ASM_ERROR_ALLOCATION_FAILED:       return "Memory allocation failed";
        case ASM_ERROR_CANNOT_OPEN_INPUT_FILE:  return "Cannot open input file";
        case ASM_ERROR_CANNOT_OPEN_OUTPUT_FILE: return "Cannot open output file";
        case ASM_ERROR_READING_FILE:            return "Error reading file";
        case ASM_ERROR_EXPECTED_ARGUMENT:       return "Expected argument for PUSH";
        case ASM_ERROR_EXPECTED_REGISTER:       return "Expected argument (register)";
        case ASM_ERROR_INVALID_REGISTER:        return "Invalid register";
        case ASM_ERROR_LABEL_TABLE:             return "Label Table Error";
        case ASM_ERROR_UNDEFINED_LABEL:         return "Undefined label";
        case ASM_ERROR_REDEFINITION_LABEL:      return "Redefinition of label";
        default:                                return "Unknown error";
    }
}
//FIXME заботать термины по типу SPU CPU что такое регистр и так далее
//FIXME написать дамп структур (проц уже есть, асма нет) и, возможно, верификатор
//FIXME раскидать кейсы по функциям
AssemblerErrorType FirstPass(Assembler* assembler_pointer) //проход только ради меток
{
    assert(assembler_pointer);

    char token[kMaxCommandLength] = {};
    int current_address = 0;
    char* buffer_ptr = assembler_pointer->instructions_buffer;

    assert(kMaxCommandLength == 32);

    while (sscanf(buffer_ptr, "%31s", token) == 1)
    {
        buffer_ptr += strlen(token);

        buffer_ptr = SkipAllSpaceSymbols(buffer_ptr);

        if (token[0] == kLabelIdSymbol)
        {
            if (AddLabel(&assembler_pointer->label_table, token + 1, current_address))
                return ASM_ERROR_LABEL_TABLE;

            continue;  // ВАЖНО!!!!!!! пропускаем увеличение адреса для меток
        }

        OpCodes operation_code = GetOpCode(token);
        if (operation_code == OP_ERR)
            return ASM_ERROR_UNKNOWN_COMMAND;

        // команды с аргументами занимают дополнительное место
        current_address += 2;

        // Если команда требует аргумента, пропускаем следующий токен
        if (CommandRequiresArgument(operation_code))
        {
            char next_token[kMaxCommandLength] = {};

            assert(kMaxCommandLength == 32);

            if (sscanf(buffer_ptr, "%31s", next_token) == 1)
            {
                buffer_ptr += strlen(next_token);
                buffer_ptr = SkipAllSpaceSymbols(buffer_ptr);
            }
            else
                return ASM_ERROR_EXPECTED_ARGUMENT;
        }
    }

    assembler_pointer->size_of_binary_file = current_address;
    return ASM_ERROR_NO;
}

bool CommandRequiresArgument(OpCodes op)
{
    switch (op) {
        case OP_PUSH:
        case OP_POPR:
        case OP_PUSHR:
        case OP_JMP:
        case OP_JB:
        case OP_JBE:
        case OP_JA:
        case OP_JAE:
        case OP_JE:
        case OP_JNE:
        case OP_CALL:
        case OP_PUSHM:
        case OP_POPM:
            return true;
        case OP_ERR:
        case OP_HLT:
        case OP_POP:
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_SQRT:
        case OP_OUT:
        case OP_IN:
        case OP_RET:
        case OP_DRAW:
        default:
            return false;
    }
}

AssemblerErrorType SecondPass(Assembler* assembler_pointer) //этот проход уже с записью в бинарный файл
{
    assert(assembler_pointer);
    assert(assembler_pointer->binary_file);

    char token[kMaxCommandLength] = {0};
    int argument = 0;
    int binary_index = 0;
    int commands_processed = 0;
    char* buffer_ptr = assembler_pointer->instructions_buffer;
    OpCodes operation_code = OP_ERR;

    assert(kMaxCommandLength == 32);

    while (sscanf(buffer_ptr, "%31s", token) == 1)
    {
        buffer_ptr += strlen(token);

        buffer_ptr = SkipAllSpaceSymbols(buffer_ptr);

        if (token[0] == kLabelIdSymbol) //проверку можно в отедельную функцию (мб через strchr, чтобы искать двоеточие в конце)
            continue;

        operation_code = GetOpCode(token);
        if (operation_code == OP_ERR)
            return ASM_ERROR_UNKNOWN_COMMAND;

        assembler_pointer->binary_buffer[binary_index++] = operation_code;

        switch(operation_code)
        {
            case OP_PUSH:
            {
                int chars_read = 0;
                if (sscanf(buffer_ptr, "%d%n", &argument, &chars_read) != 1)
                    return ASM_ERROR_EXPECTED_ARGUMENT;

                assembler_pointer->binary_buffer[binary_index++] = argument;
                buffer_ptr += chars_read; //сдвигаем указатель в буфере инструкций на длину этой строки
                buffer_ptr = SkipAllSpaceSymbols(buffer_ptr);
                break;
            }

            case OP_JMP:
            case OP_JB:
            case OP_JBE:
            case OP_JA:
            case OP_JAE:
            case OP_JE:
            case OP_JNE:
            case OP_CALL:
                {
                    char label_name[kMaxLabelLength] = {0};

                    assert(kMaxCommandLength == 32);

                    if (sscanf(buffer_ptr, "%31s", label_name) == 1)
                    {
                        int label_address = FindLabel(&assembler_pointer->label_table, label_name);
                        if (label_address == -1)
                            return ASM_ERROR_UNDEFINED_LABEL;

                        assembler_pointer->binary_buffer[binary_index++] = label_address;
                        buffer_ptr += strlen(label_name);
                        buffer_ptr = SkipAllSpaceSymbols(buffer_ptr);
                    }
                    else
                        return ASM_ERROR_EXPECTED_ARGUMENT;

                    break;
                }
            case OP_RET:
                assembler_pointer->binary_buffer[binary_index++] = 0;
                break;

            case OP_POPR:
            case OP_PUSHR:
                {
                    buffer_ptr = SkipAllSpaceSymbols(buffer_ptr);

                    char register_name[kMaxCommandLength] = {};

                    assert(kMaxCommandLength == 32);

                    int read_count = sscanf(buffer_ptr, "%31s", register_name);

                    if (read_count != 1)
                        return ASM_ERROR_EXPECTED_REGISTER;

                    RegCodes reg = GetRegisterByName(register_name);
                    if (reg == REG_INVALID)
                        return ASM_ERROR_INVALID_REGISTER; //мб тут вывести еще регистры, которые пользователь может использовать

                    assembler_pointer->binary_buffer[binary_index++] = (int) reg;
                    buffer_ptr += strlen(register_name);
                    buffer_ptr = SkipAllSpaceSymbols(buffer_ptr);
                    break;
                }

            case OP_PUSHM:
            case OP_POPM:
            {
                buffer_ptr = SkipAllSpaceSymbols(buffer_ptr);

                if (*buffer_ptr != '[')
                    return ASM_ERROR_EXPECTED_ARGUMENT;
                buffer_ptr++;

                char* closing_bracket = strchr(buffer_ptr, ']');
                if (closing_bracket == NULL)
                    return ASM_ERROR_EXPECTED_ARGUMENT;

                char register_name[kMaxCommandLength] = {};
                int reg_name_length = closing_bracket - buffer_ptr;
                if (reg_name_length >= sizeof(register_name))
                    return ASM_ERROR_INVALID_REGISTER;

                strncpy(register_name, buffer_ptr, reg_name_length);

                RegCodes reg = GetRegisterByName(register_name);
                if (reg == REG_INVALID)
                    return ASM_ERROR_INVALID_REGISTER; //мб тут вывести еще регистры, которые пользователь может использовать

                assembler_pointer->binary_buffer[binary_index++] = (int) reg;
                buffer_ptr = closing_bracket + 1;
                buffer_ptr = SkipAllSpaceSymbols(buffer_ptr);
                break;
            }

            case OP_HLT:
            case OP_ADD:
            case OP_POP:
            case OP_SUB:
            case OP_MUL:
            case OP_DIV:
            case OP_SQRT:
            case OP_OUT:
            case OP_IN:
            case OP_DRAW:
                assembler_pointer->binary_buffer[binary_index++] = 0;
                break;

            case OP_ERR:
            default:
                return ASM_ERROR_UNKNOWN_COMMAND;
        }

        commands_processed++;
    }
    AssemblerErrorType error = WriteBinaryBufferToBinaryFile(assembler_pointer, binary_index);
    if (error != ASM_ERROR_NO)
        return error;

    printf("Processed %d commands\n", commands_processed);
    return ASM_ERROR_NO;

}

AssemblerErrorType WriteBinaryBufferToBinaryFile(Assembler* assembler_pointer, int number_of_ints)
{
    assert(assembler_pointer);
    assert(number_of_ints >= 0);

    if (!assembler_pointer->binary_file)
        return ASM_ERROR_CANNOT_OPEN_OUTPUT_FILE;

    fwrite(assembler_pointer->binary_buffer, sizeof(assembler_pointer->binary_buffer[0]), number_of_ints, assembler_pointer->binary_file);

    return ASM_ERROR_NO;
}

AssemblerErrorType GetInstructionFileFileAndReadItToBuffer(Assembler* assembler_pointer, const char* input_filename)
{
    assert(assembler_pointer);
    assert(input_filename);

    FILE* instruction_file = GetInputFile(input_filename);
    if (!instruction_file)
        return ASM_ERROR_CANNOT_OPEN_INPUT_FILE;

    long file_size = GetFileSize(instruction_file);
    if (file_size < 0)
    {
        fclose(instruction_file);
        return ASM_ERROR_READING_FILE;
    }

    assembler_pointer->inst_buffer_size = file_size + sizeof('\0');
    assembler_pointer->instructions_buffer = (char*)calloc(assembler_pointer->inst_buffer_size, sizeof(char));
    if (!assembler_pointer->instructions_buffer)
    {
        fclose(instruction_file);
        return ASM_ERROR_ALLOCATION_FAILED;
    }

    size_t bytes_read = fread(assembler_pointer->instructions_buffer, sizeof(char), file_size, instruction_file);
    if (bytes_read != (size_t)file_size)
    {
        fclose(instruction_file);
        FREE_AND_NULL(assembler_pointer->instructions_buffer);

        return ASM_ERROR_READING_FILE;
    }

    assembler_pointer->instructions_buffer[file_size] = '\0';
    fclose(instruction_file);

    return ASM_ERROR_NO;
}

AssemblerErrorType AssemblerCtor(Assembler* assembler_pointer, const char* input_filename, const char* output_filename)
{
    assert(assembler_pointer);
    assert(input_filename);
    assert(output_filename);

    assembler_pointer->instruction_filename = input_filename;
    assembler_pointer->binary_filename      = output_filename;

    assembler_pointer -> binary_file = GetOutputFile(output_filename);
    if (!assembler_pointer -> binary_file)
        return ASM_ERROR_CANNOT_OPEN_OUTPUT_FILE;

    AssemblerErrorType error = GetInstructionFileFileAndReadItToBuffer(assembler_pointer, input_filename);
    if (error != ASM_ERROR_NO)
    {
        fclose(assembler_pointer->binary_file);
        assembler_pointer->binary_file = NULL;
        return error;
    }

    size_t max_possible_commands = assembler_pointer->inst_buffer_size / 2 + 1;
    assembler_pointer->binary_buffer = (int*)calloc(max_possible_commands, sizeof(int));
    if (!assembler_pointer->binary_buffer)
    {
        FREE_AND_NULL(assembler_pointer->instructions_buffer);

        fclose(assembler_pointer->binary_file);
        assembler_pointer->binary_file = NULL;
        return ASM_ERROR_ALLOCATION_FAILED;
    }

    assembler_pointer->label_table.labels = (Label*) calloc(kMaxNOfLabels, sizeof(Label));
    InitLabelTable(&assembler_pointer->label_table);

    return ASM_ERROR_NO;
}

void AssemblerDtor(Assembler* assembler_pointer)
{
    if (!assembler_pointer)
        return;

    FREE_AND_NULL(assembler_pointer->instructions_buffer);
    FREE_AND_NULL(assembler_pointer->binary_buffer);
    FREE_AND_NULL(assembler_pointer->label_table.labels);

    if (assembler_pointer->binary_file)
        fclose(assembler_pointer->binary_file);

    assembler_pointer->inst_buffer_size = 0;
    assembler_pointer->instruction_filename = NULL;
    assembler_pointer->binary_filename = NULL;
    assembler_pointer->binary_file = NULL;
}

FILE* GetInputFile(const char* instruction_file_path)
{
    assert(instruction_file_path);

    FILE* instruction_file = fopen(instruction_file_path, "r");
    if (instruction_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open input file %s\n", instruction_file_path);
        return NULL;
    }
    return instruction_file;
}

FILE* GetOutputFile(const char* binary_file_path)
{
    assert(binary_file_path);

    FILE* binary_file = fopen(binary_file_path, "wb");
    if (binary_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open output file %s\n", binary_file_path);
        return NULL;
    }
    return binary_file;
}

RegCodes GetRegisterByName(const char* name)
{
    assert(name);

    const char* register_names[] = {"RAX", "RBX", "RCX", "RDX", "REX", "RFX", "RGX", "RHX"};

    assert(sizeof(register_names) / sizeof(register_names[0]) == kNRegisters);

    for (int i = 0; i < kNRegisters; i++)
    {
        if (strcmp(name, register_names[i]) == 0)
            return (RegCodes) i;
    }
    return REG_INVALID;
}


void InitLabelTable(LabelTable* ptr_table)
{
    assert(ptr_table);

    ptr_table->number_of_labels = 0;
    for (int index_of_label = 0; index_of_label < kMaxNOfLabels; index_of_label++)
    {
        ptr_table->labels[index_of_label].name[0] = '\0';
        ptr_table->labels[index_of_label].address = -1;
    }
}

int FindLabel(LabelTable* ptr_table, const char* name_of_label)
{
    assert(ptr_table);
    assert(name_of_label);

    for (int i = 0; i < ptr_table->number_of_labels; i++)
    {
        if (strcmp(ptr_table->labels[i].name, name_of_label) == 0)
            return ptr_table->labels[i].address;
    }
    return -1;
}

AssemblerErrorType AddLabel(LabelTable* ptr_table, const char* name_of_label, int address)
{
    assert(ptr_table);
    assert(name_of_label);

    if (ptr_table->number_of_labels >= kMaxNOfLabels)
        return ASM_ERROR_LABEL_TABLE;

    if (FindLabel(ptr_table, name_of_label) != -1) //вдруг уже есть такая метка
        return ASM_ERROR_REDEFINITION_LABEL;

    strncpy(ptr_table->labels[ptr_table->number_of_labels].name, name_of_label, kMaxLabelLength - 1);
    ptr_table->labels[ptr_table->number_of_labels].address = address;
    ptr_table->number_of_labels++;

    return ASM_ERROR_NO;
}

char* SkipAllSpaceSymbols(char* buffer_ptr)
{
    while (isspace(*buffer_ptr))
        buffer_ptr++;

    return buffer_ptr;
}
