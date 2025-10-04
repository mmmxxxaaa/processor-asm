#include "my_assembler.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "asm_error_types.h"

OpCodes GetOpCode(const char* command)
{
    assert(command != NULL);

    // сделать макрос, который принимает на вход имя команды и раскрывается в такой if с помощью ##, типо OP_ ## HLT
    if (strcmp(command, "HLT") == 0)
        return OP_HLT;
    if (strcmp(command, "PUSH") == 0)
        return OP_PUSH;
    if (strcmp(command, "POP") == 0)
        return OP_POP;
    if (strcmp(command, "ADD") == 0)
        return OP_ADD;
    if (strcmp(command, "SUB") == 0)
        return OP_SUB;
    if (strcmp(command, "MUL") == 0)
        return OP_MUL;
    if (strcmp(command, "DIV") == 0)
        return OP_DIV;
    if (strcmp(command, "SQRT") == 0)
        return OP_SQRT;
    if (strcmp(command, "OUT") == 0)
        return OP_OUT;
    fprintf(stderr, "Error: Unknown command '%s'\n", command);
    return OP_ERR;
}



int ReadOpCodesFromInstructionFileAndPutThemToBinaryFile(const char* instruction_filename, const char* binary_filename)
{
    assert(instruction_filename != NULL);
    assert(binary_filename != NULL);

    FILE* instruction_file = fopen(instruction_filename, "r");
    if (instruction_file == NULL)
    {
        printf("Error: Cannot open input file %s\n", instruction_filename);
        return -1;
    }

    FILE* binary_file = fopen(binary_filename, "wb");
    if (binary_file == NULL)
    {
        printf("Error: Cannot open input file %s\n", binary_filename);
        return -1;
    }

    // массив команд сделать динамическим, а то вдруг в него всё не влезет
    char command[MAX_COMMAND_LENGTH] = {0};
    int argument = 0;
    int counter_of_arguments = 0;
    OpCodes operation_code = OP_ERR; //ДЕЛО СДЕЛАНО норм инициализировал же?
    while (fscanf(instruction_file, "%s", command) != EOF)
    {
        operation_code = GetOpCode(command);
        fwrite(&operation_code, sizeof(int), 1, binary_file);
        switch(operation_code)
        {
            case OP_PUSH:
                if (fscanf(instruction_file, "%d", &argument) == 1)
                {
                    fwrite(&argument, sizeof(int), 1, binary_file);
                }
                else
                {
                    fprintf(stderr, "Error: expected argument for PUSH\n"); //FIXME обработку ошибок можно засунуть в отдельную функцию
                    fclose(instruction_file);
                    fclose(binary_file);
                    return -1;
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
                // Я выбрал записывать аргумент для всех команд, чтобы потом просто по 8 байт читать
                argument = 0;
                fwrite(&argument, sizeof(int), 1, binary_file);
                break;

            default:
                //FIXME что делать с неизвестными командами?
                argument = 0;
                fwrite(&argument, sizeof(int), 1, binary_file);
                break;
        }

        counter_of_arguments += 1;

        if (operation_code == OP_HLT)
            break;
    }

// ДобавитьДеструктор
    fclose(instruction_file);
    fclose(binary_file);

    printf("Processed %d commands\n", counter_of_arguments);
    return counter_of_arguments;
}


int AssemblerCtor(Assembler* assembler_pointer, const char* input_filename, const char* output_filename, size_t starting_capacity)
{
    assert(assembler_pointer);
    assert(input_filename);
    assert(output_filename);

    assembler_pointer -> instruction_filename = input_filename;
    assembler_pointer -> binary_filename = output_filename;
    assembler_pointer -> instructions_buffer = (char*) calloc(starting_capacity, sizeof(char));
    assembler_pointer -> binary_buffer       = (int*) calloc(starting_capacity * 4, sizeof(int));
    //FIXME добавить enum ошибок асма и написать AssemblerDump
    // if (assembler_pointer -> instructions_buffer == NULL)
    // {
    //     int errors = ERROR_ALLOCATION_FAILED;
    //     AssemblerDump(assembler_pointer, errors, "Allocating memory in AssembleCtor failed");
    //     return errors;
    // }

    // if (assembler_pointer -> binary_buffer == NULL)
    // {
    //     int errors = ERROR_ALLOCATION_FAILED;
    //     AssemblerDump(assembler_pointer, errors, "Allocating memory in AssembleCtor failed");
    //     return errors;
    // }

    assembler_pointer -> instruction_file = NULL;
    assembler_pointer -> binary_file = NULL;

    return ERROR_NO;
}

void AssemblerDtor(Assembler* assembler_pointer)
{
    if (assembler_pointer -> instructions_buffer)
        free(assembler_pointer -> instructions_buffer);

    if (assembler_pointer -> binary_buffer)
        free(assembler_pointer -> binary_buffer);

    assembler_pointer -> instruction_filename = NULL;
    assembler_pointer -> binary_filename = NULL;
    assembler_pointer -> instruction_file = NULL;
    assembler_pointer -> binary_file = NULL;
}
