#include "my_assembler.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "asm_error_types.h"

// #include "general_const_and_func.h"

#define COMPARE_COMMAND(cmd, name) if (strcmp(command, #name) == 0) return OP_##name

OpCodes GetOpCode(const char* command)
{
    assert(command != NULL);
    // сделать макрос, который принимает на вход имя команды и раскрывается в такой if с помощью ##, типо OP_ ## HLT
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

    COMPARE_COMMAND(command, PUSHR);
    COMPARE_COMMAND(command, POPR);

    fprintf(stderr, "Error: Unknown command '%s'\n", command);
    return OP_ERR;
}

const char* GetAsmErrorString(AssemblerErrorType error)
{
    switch (error) {
        case ASM_ERROR_NO: return "No error";
        case ASM_ERROR_UNKNOWN_COMMAND: return "Unknown command";
        case ASM_ERROR_ALLOCATION_FAILED: return "Memory allocation failed";
        case ASM_ERROR_CANNOT_OPEN_INPUT_FILE: return "Cannot open input file";
        case ASM_ERROR_CANNOT_OPEN_OUTPUT_FILE: return "Cannot open output file";
        case ASM_ERROR_READING_FILE: return "Error reading file";
        case ASM_ERROR_EXPECTED_ARGUMENT: return "Expected argument for PUSH";
        case ASM_ERROR_EXPECTED_REGISTER: return "Expected argument (register)";
        case ASM_ERROR_INVALID_REGISTER: return "Invalid register";
        default: return "Unknown error";
    }
}

AssemblerErrorType ReadOpCodesFromInstructionFileAndPutThemToBinaryFile(Assembler* assembler_pointer)
{
    assert(assembler_pointer);
    assert(assembler_pointer->binary_file);

    char command_name[kMaxCommandLength] = {0};
    int argument = 0;
    int binary_index = 0;
    int commands_processed = 0;
    char* buffer_ptr = assembler_pointer->instructions_buffer;
    OpCodes operation_code = OP_ERR;

    while (sscanf(buffer_ptr, "%s", command_name) == 1)
    {
        buffer_ptr += strlen(command_name);

        while (isspace(*buffer_ptr))
            buffer_ptr++;

        operation_code = GetOpCode(command_name);
        if (operation_code == OP_ERR) //в кейс
            return ASM_ERROR_UNKNOWN_COMMAND;

        assembler_pointer->binary_buffer[binary_index++] = operation_code; //записываем коды операций в бинарный буфер

        switch(operation_code)
        {
            case OP_PUSH:
            case OP_JMP:
            case OP_JB:
            case OP_JBE:
            case OP_JA:
            case OP_JAE:
            case OP_JE:
            case OP_JNE:
                if (sscanf(buffer_ptr, "%d", &argument) == 1) //фигурные скобки нужны, чтобы создать переменную внутри кейса
                {
                    assembler_pointer->binary_buffer[binary_index++] = argument; // записываем аргумент в бинарный буфер

            // FIXME - Вместо сдвига на размер числа strchr('\n')
                    char arg_buffer[32] = {0}; //создаем временный буфер для строкового представления числа
                    sprintf(arg_buffer, "%d", argument); //преобразуем число обратно в строку с помощью sprintf
                    buffer_ptr += strlen(arg_buffer); //сдвигаем указатель в буфере инструкций на длину этой строки
                    while ((*buffer_ptr == ' ') || (*buffer_ptr == '\n') || (*buffer_ptr == '\r') || (*buffer_ptr == '\t'))// Пропускаем пробелы после аргумента
                        buffer_ptr++;
                }
                else
                {
                    return ASM_ERROR_EXPECTED_ARGUMENT;
                }
                break;

            case OP_HLT:
            case OP_ADD:
            case OP_POP:
            case OP_SUB:
            case OP_MUL:
            case OP_DIV:
            case OP_SQRT:
            case OP_OUT:
            case OP_IN:
                assembler_pointer->binary_buffer[binary_index++] = 0;
                break;

            case OP_POPR:
            case OP_PUSHR:
            {
                while (*buffer_ptr == ' ' || *buffer_ptr == '\t') {
                    buffer_ptr++;
                }

                char register_name[32] = {};
                int read_count = sscanf(buffer_ptr, "%31s", register_name);

                if (read_count != 1)
                    return ASM_ERROR_EXPECTED_REGISTER;

                RegCodes reg = GetRegisterByName(register_name);
                if (reg == REG_INVALID)
                    return ASM_ERROR_INVALID_REGISTER; //мб тут вывести еще регистры, которые пользователь может использовать

                assembler_pointer->binary_buffer[binary_index++] = (int) reg;
                buffer_ptr += strlen(register_name);

                while (isspace(*buffer_ptr))
                    buffer_ptr++;
                break;
            }

            case OP_ERR:
            default:
                return ASM_ERROR_UNKNOWN_COMMAND;
        }

        commands_processed++;

        if (operation_code == OP_HLT)
            break;
    }
    fwrite(assembler_pointer->binary_buffer, sizeof(int), binary_index, assembler_pointer->binary_file);

    printf("Processed %d commands\n", commands_processed);
    return ASM_ERROR_NO;
}

AssemblerErrorType ReadInstructionFileToBuffer(Assembler* assembler_pointer, const char* input_filename)
{//assert
    FILE* instruction_file = GetInputFile(input_filename);
    if (!instruction_file)
    {
        return ASM_ERROR_CANNOT_OPEN_INPUT_FILE;
    }

    long file_size = GetFileSize(instruction_file);
    if (file_size < 0)
    {
        fclose(instruction_file);
        return ASM_ERROR_READING_FILE;
    }

    assembler_pointer->instructions_buffer = (char*)calloc(file_size + 1, sizeof(char)); //+1, чтобы \0 поместился
    if (!assembler_pointer->instructions_buffer)
    {
        fclose(instruction_file);
        return ASM_ERROR_ALLOCATION_FAILED;
    }

    size_t bytes_read = fread(assembler_pointer->instructions_buffer, 1, file_size, instruction_file);
    if (bytes_read != (size_t)file_size)
    {
        fclose(instruction_file);
        free(assembler_pointer->instructions_buffer);
        assembler_pointer->instructions_buffer = NULL;
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

    assembler_pointer -> binary_file = GetOutputFile(output_filename);
    if (!assembler_pointer->binary_file)
        return ASM_ERROR_CANNOT_OPEN_OUTPUT_FILE;

    FILE* instruction_file = GetInputFile(input_filename);
    if (!instruction_file)
        return ASM_ERROR_CANNOT_OPEN_INPUT_FILE;

    AssemblerErrorType error = ReadInstructionFileToBuffer(assembler_pointer, input_filename);
    if (error != ASM_ERROR_NO)
    {
        fclose(assembler_pointer->binary_file);
        assembler_pointer->binary_file = NULL;
        return error;
    }

    size_t max_possible_commands = strlen(assembler_pointer->instructions_buffer) / 2 + 1; //оценка
    assembler_pointer->binary_buffer = (int*)calloc(max_possible_commands, sizeof(int));
    if (!assembler_pointer->binary_buffer)
    {
        free(assembler_pointer->instructions_buffer);
        assembler_pointer->instructions_buffer = NULL;
        fclose(instruction_file);
        return ASM_ERROR_ALLOCATION_FAILED;
    }

    assembler_pointer -> instruction_filename = strdup(input_filename);
    assembler_pointer -> binary_filename      = strdup(output_filename);

    if (!assembler_pointer->instruction_filename || !assembler_pointer->binary_filename)
    {
        AssemblerDtor(assembler_pointer);
        return ASM_ERROR_ALLOCATION_FAILED;
    }

    return ASM_ERROR_NO;
}

void AssemblerDtor(Assembler* assembler_pointer)
{
    if (!assembler_pointer)
        return;

    // Освобождаем только если указатели не NULL
    if (assembler_pointer->instructions_buffer)
        free(assembler_pointer->instructions_buffer);

    if (assembler_pointer->binary_buffer)
        free(assembler_pointer->binary_buffer);

    if (assembler_pointer->binary_file)
        fclose(assembler_pointer->binary_file);

    if (assembler_pointer->instruction_filename)
        free(assembler_pointer->instruction_filename);

    if (assembler_pointer->binary_filename)
        free(assembler_pointer->binary_filename);


    assembler_pointer->instruction_filename = NULL;
    assembler_pointer->binary_filename = NULL;
    assembler_pointer->instructions_buffer = NULL;
    assembler_pointer->binary_buffer = NULL;
    assembler_pointer->binary_file = NULL;
}

FILE* GetInputFile(const char* instruction_filename)
{
    FILE* instruction_file = fopen("../my_text_instructions.txt", "r");
    if (instruction_file == NULL)
    {
        printf("Error: Cannot open input file %s\n", instruction_filename);
        return NULL;
    }
    return instruction_file;
}

FILE* GetOutputFile(const char* binary_filename)
{
    FILE* binary_file = fopen(binary_filename, "wb");
    if (binary_file == NULL)
    {
        printf("Error: Cannot open input file %s\n", binary_filename);
        return NULL;
    }
    return binary_file;
}

long int GetFileSize(FILE* file)
{
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return file_size;
}

const char* GetRegisterName(RegCodes reg)
{
    const char* register_names[] = {"RAX", "RBX", "RCX", "RDX", "REX", "RFX", "RGX", "RHX"};

    if (reg >= REG_RAX && reg <= REG_RHX) {
        return register_names[reg];
    }
    return "UNKNOWN";
}

RegCodes GetRegisterByName(const char* name) {
    const char* register_names[] = {
        "RAX", "RBX", "RCX", "RDX", "REX", "RFX", "RGX", "RHX"
    };

    for (int i = 0; i < kNRegisters; i++) {
        if (strcmp(name, register_names[i]) == 0) {
            return (RegCodes) i;
        }
    }
    return REG_INVALID;
}

int IsValidRegister(RegCodes reg)
{
    return (reg >= REG_RAX && reg <= REG_RHX);
}
