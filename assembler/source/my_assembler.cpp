#include "my_assembler.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
//упорядочили хеши qsortом
//дальше если совпали хеши упорядочиваем по строкам
//дальше бинарный поиск
#include "asm_error_types.h"

//FIXME
// в массив структур описаний комманд добавить поле хеш название команды и для каждой введенной команды его подсчитывать
// в начале ассемблирования вызвать функ инициализатор которая посчитает хеш для каждой и запишет это в поле структуры после чего надо отсортировать массив структур по возрастанию 1 раз(в самом начале запуска)
// при чтении команды я ее хеширую а потом двоичным поиском ищу в массиве структур(по хешам)  быстый поиск команд

// завести поле -- тип аргумента. Анализировать это поле в ходе ассемблирования
// если поле говорит о том, что требуется аргумент, то нужно его прочитать и сделать все нужное
// после этого перейти к след команде
// лучше создать отдельную функцию для обработки аргументов
// принимать тип аргумента, в зависимости от которого она будет его обрабатывать
// sscanf

static CommandInfo command_infos[] = { //FIXME не могу сделать его константным, тогда надо делать его статик в cpp?
    {"HLT",   0, OP_HLT,   ARG_NONE    },
    {"PUSH",  0, OP_PUSH,  ARG_NUMBER  },
    {"POP",   0, OP_POP,   ARG_NONE    },
    {"ADD",   0, OP_ADD,   ARG_NONE    },
    {"SUB",   0, OP_SUB,   ARG_NONE    },
    {"MUL",   0, OP_MUL,   ARG_NONE    },
    {"DIV",   0, OP_DIV,   ARG_NONE    },
    {"SQRT",  0, OP_SQRT,  ARG_NONE    },
    {"IN",    0, OP_IN,    ARG_NONE    },
    {"OUT",   0, OP_OUT,   ARG_NONE    },
    {"JMP",   0, OP_JMP,   ARG_LABEL   },
    {"JB",    0, OP_JB,    ARG_LABEL   },
    {"JBE",   0, OP_JBE,   ARG_LABEL   },
    {"JA",    0, OP_JA,    ARG_LABEL   },
    {"JAE",   0, OP_JAE,   ARG_LABEL   },
    {"JE",    0, OP_JE,    ARG_LABEL   },
    {"JNE",   0, OP_JNE,   ARG_LABEL   },
    {"CALL",  0, OP_CALL,  ARG_LABEL   },
    {"RET",   0, OP_RET,   ARG_NONE    },
    {"PUSHM", 0, OP_PUSHM, ARG_MEMORY  },
    {"POPM",  0, OP_POPM,  ARG_MEMORY  },
    {"DRAW",  0, OP_DRAW,  ARG_NONE    },
    {"PUSHR", 0, OP_PUSHR, ARG_REGISTER},
    {"POPR",  0, OP_POPR,  ARG_REGISTER}
};

static size_t n_of_commands = sizeof(command_infos) / sizeof(command_infos[0]);

int CompareCommandInfos(const void* first_cmd, const void* second_cmd) //компаратор для qsort
{
    const CommandInfo* cmdA = (const CommandInfo*) first_cmd;
    const CommandInfo* cmdB = (const CommandInfo*) second_cmd;

    if (cmdA->hash != cmdB->hash)
        return (cmdA->hash < cmdB->hash) ? -1 : 1;

    return strcmp(cmdA->name, cmdB->name);
}

unsigned int ComputeHash(const char* str) //djb2
{
    unsigned int hash = 5381;
    int c = 0;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; //умножаем на 33 без умножения
    return hash;
}

void InitializeCommandInfos()
{
    for (size_t i = 0; i < n_of_commands; i++)
        command_infos[i].hash = ComputeHash(command_infos[i].name);

    qsort(command_infos, n_of_commands, sizeof(CommandInfo), CompareCommandInfos); //FIXME
}

CommandInfo* FindCommandByHash(unsigned int hash, const char* name)
{
    size_t left = 0;
    size_t right = n_of_commands;

    while (left < right)
    {
        size_t middle = left + (right - left) / 2;

        if (command_infos[middle].hash < hash)
            left = middle + 1;
        else if (command_infos[middle].hash > hash)
            right = middle;
        else //FIXME вроде не насрал
        {
            // нашли хеш, а теперь проверим соседние элементы на коллизии
            size_t same_hash_start = middle;
            size_t same_hash_end = middle;

            // начало блока с одинаковыми хешами
            while (same_hash_start > 0 && command_infos[same_hash_start - 1].hash == hash)
                same_hash_start--;

            // конец блока с одинаковыми хешами
            while (same_hash_end < n_of_commands - 1 && command_infos[same_hash_end + 1].hash == hash)
                same_hash_end++;

            // если блок состоит из одного элемента, то коллизий нет
            if (same_hash_start == same_hash_end)
                return &command_infos[middle];

            // сли есть коллизии, тогда ищем по имени
            for (size_t i = same_hash_start; i <= same_hash_end; i++)
            {
                if (strcmp(command_infos[i].name, name) == 0)
                    return &command_infos[i];
            }
            return NULL;
        }
    }
    return NULL; // не нашли хещ
}

OpCodes GetOpCode(const char* command) //FIXME
{
    assert(command != NULL);

    static int initialized = 0;
    if (!initialized)
    {
        InitializeCommandInfos();
        initialized = 1;
    }

    unsigned int current_hash = ComputeHash(command);
    CommandInfo* current_cmd_info = FindCommandByHash(current_hash, command);

    if (current_cmd_info != NULL)
        return current_cmd_info->opcode;

    fprintf(stderr, "Error: Unknown command '%s'\n", command);
    return OP_ERR;
}

ArgumentType GetArgumentType(OpCodes op)
{
    for (size_t i = 0; i < n_of_commands; i++)
    {
        if (command_infos[i].opcode == op)
            return command_infos[i].arg_type;
    }
    return ARG_NONE;
}

AssemblerErrorType ProcessNumberArgument(Assembler* assembler_pointer, char** buffer_ptr, int* binary_index)  //FIXME то же, что и было в кейсе пуша
{
    int argument = 0;
    int chars_read = 0;

    if (sscanf(*buffer_ptr, "%d%n", &argument, &chars_read) != 1)
        return ASM_ERROR_EXPECTED_ARGUMENT;

    assembler_pointer->binary_buffer[(*binary_index)++] = argument;
    *buffer_ptr += chars_read;
    *buffer_ptr = SkipAllSpaceSymbols(*buffer_ptr);

    return ASM_ERROR_NO;
}

AssemblerErrorType ProcessLabelArgument(Assembler* assembler_pointer, char** buffer_ptr, int* binary_index) //FIXME
{
    char label_name[kMaxLabelLength] = {0};

    assert(kMaxLabelLength == 32);

    if (sscanf(*buffer_ptr, "%31s", label_name) != 1)
        return ASM_ERROR_EXPECTED_ARGUMENT;

    int label_address = FindLabel(&assembler_pointer->label_table, label_name);
    if (label_address == -1)
        return ASM_ERROR_UNDEFINED_LABEL;

    assembler_pointer->binary_buffer[(*binary_index)++] = label_address;
    *buffer_ptr += strlen(label_name);
    *buffer_ptr = SkipAllSpaceSymbols(*buffer_ptr);

    return ASM_ERROR_NO;
}

AssemblerErrorType ProcessRegisterArgument(Assembler* assembler_pointer, char** buffer_ptr, int* binary_index)//FIXME
{
    char register_name[kMaxCommandLength] = {0};

    assert(kMaxCommandLength == 32);

    int read_count = sscanf(*buffer_ptr, "%31s", register_name);
    if (read_count != 1)
        return ASM_ERROR_EXPECTED_REGISTER;

    RegCodes reg = GetRegisterByName(register_name);
    if (reg == REG_INVALID)
        return ASM_ERROR_INVALID_REGISTER;

    assembler_pointer->binary_buffer[(*binary_index)++] = (int)reg;
    *buffer_ptr += strlen(register_name);
    *buffer_ptr = SkipAllSpaceSymbols(*buffer_ptr);

    return ASM_ERROR_NO;
}

AssemblerErrorType ProcessMemoryArgument(Assembler* assembler_pointer, char** buffer_ptr, int* binary_index) //FIXME
{
    *buffer_ptr = SkipAllSpaceSymbols(*buffer_ptr);

    if (**buffer_ptr != '[')
        return ASM_ERROR_EXPECTED_ARGUMENT;
    (*buffer_ptr)++;

    char* closing_bracket = strchr(*buffer_ptr, ']');
    if (closing_bracket == NULL)
        return ASM_ERROR_EXPECTED_ARGUMENT;

    char register_name[kMaxCommandLength] = {};
    int reg_name_length = closing_bracket - *buffer_ptr;
    if (reg_name_length >= (int)sizeof(register_name))
        return ASM_ERROR_INVALID_REGISTER;

    strncpy(register_name, *buffer_ptr, reg_name_length);
    register_name[reg_name_length] = '\0';

    RegCodes reg = GetRegisterByName(register_name);
    if (reg == REG_INVALID)
        return ASM_ERROR_INVALID_REGISTER;

    assembler_pointer->binary_buffer[(*binary_index)++] = (int)reg;
    *buffer_ptr = closing_bracket + 1;
    *buffer_ptr = SkipAllSpaceSymbols(*buffer_ptr);

    return ASM_ERROR_NO;
}

AssemblerErrorType ProcessNoArgument(Assembler* assembler_pointer, int* binary_index) //FIXME
{
    assembler_pointer->binary_buffer[(*binary_index)++] = 0;
    return ASM_ERROR_NO;
}

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

AssemblerErrorType SecondPass(Assembler* assembler_pointer) //этот проход уже с записью в бинарный файл
{
    assert(assembler_pointer);
    assert(assembler_pointer->binary_file);

    char token[kMaxCommandLength] = {0};
    int binary_index = 0;
    int commands_processed = 0;
    char* buffer_ptr = assembler_pointer->instructions_buffer;
    // OpCodes operation_code = OP_ERR;

    assert(kMaxCommandLength == 32);

    while (sscanf(buffer_ptr, "%31s", token) == 1)
    {
        buffer_ptr += strlen(token);
        buffer_ptr = SkipAllSpaceSymbols(buffer_ptr);

        if (token[0] == kLabelIdSymbol) //проверку можно в отедельную функцию (мб через strchr, чтобы искать двоеточие в конце)
            continue;

        unsigned int hash = ComputeHash(token);
        CommandInfo* cmd_info = FindCommandByHash(hash, token);

        if (cmd_info == NULL)
            return ASM_ERROR_UNKNOWN_COMMAND;

        OpCodes operation_code = cmd_info->opcode;
        ArgumentType arg_type = cmd_info->arg_type;

        assembler_pointer->binary_buffer[binary_index++] = operation_code;

        AssemblerErrorType arg_error = ASM_ERROR_NO;
        switch (arg_type) //FIXME теперь у меня свитч кейс для аргументов
        {
            case ARG_NUMBER:
                arg_error = ProcessNumberArgument(assembler_pointer, &buffer_ptr, &binary_index);
                break;
            case ARG_LABEL:
                arg_error = ProcessLabelArgument(assembler_pointer, &buffer_ptr, &binary_index);
                break;
            case ARG_REGISTER:
                arg_error = ProcessRegisterArgument(assembler_pointer, &buffer_ptr, &binary_index);
                break;
            case ARG_MEMORY:
                arg_error = ProcessMemoryArgument(assembler_pointer, &buffer_ptr, &binary_index);
                break;
            case ARG_NONE:
                arg_error = ProcessNoArgument(assembler_pointer, &binary_index);
                break;
            default:
                return ASM_ERROR_UNKNOWN_COMMAND;
        }
        if (arg_error != ASM_ERROR_NO)
            return arg_error;

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

bool CommandRequiresArgument(OpCodes op) //FIXME
{
    ArgumentType arg_type = GetArgumentType(op);
    return (arg_type != ARG_NONE);
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
