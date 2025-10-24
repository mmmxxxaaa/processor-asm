#ifndef MY_ASSEMBLER_H_
#define MY_ASSEMBLER_H_

#include <stdio.h>
#include <stdbool.h>
#include "asm_error_types.h"

#include "general_const_and_func.h"

const int  kMaxCommandLength = 32;
const int  kMaxNOfLabels = 100;
const int  kMaxLabelLength = 32;
const char kLabelIdSymbol = ':';

typedef enum {
    REG_INVALID = -1,
    REG_RAX = 0,
    REG_RBX = 1,
    REG_RCX = 2,
    REG_RDX = 3,
    REG_REX = 4,
    REG_RFX = 5,
    REG_RGX = 6,
    REG_RHX = 7,
} RegCodes;

typedef enum {
    ARG_NONE     = 0,
    ARG_NUMBER   = 1,
    ARG_LABEL    = 2,
    ARG_REGISTER = 3,
    ARG_MEMORY   = 4
} ArgumentType;

typedef struct {
    char name[kMaxLabelLength];
    int  address;
} Label;

typedef struct {
    Label* labels;
    int    number_of_labels;
} LabelTable;

typedef struct {
    const char* instruction_filename; // имя файла на вход и указатель на него //нельзя конст
    char*       instructions_buffer; // буффер с текстом из входного файла, вводится один раз
    int         inst_buffer_size;
    const char* binary_filename; // имя файла на выход и указатель на него     //нельзя конст
    FILE*       binary_file;
    int*        binary_buffer;
    int         size_of_binary_file;
    LabelTable  label_table;
} Assembler;

typedef struct {
    const char* name;
    unsigned int hash;
    OpCodes opcode;
    ArgumentType arg_type;
#endif
} CommandInfo;

-D PROC

//FIXME
int CompareCommandInfos(const void* first_cmd, const void* second_cmd);
unsigned int ComputeHash(const char* str);
void InitializeCommandInfos();
CommandInfo* FindCommandByHash(unsigned int hash, const char* name);
OpCodes GetOpCode(const char* command);
ArgumentType GetArgumentType(OpCodes op);
AssemblerErrorType ProcessNumberArgument  (Assembler* assembler_pointer, char** buffer_ptr, int* binary_index);
AssemblerErrorType ProcessLabelArgument   (Assembler* assembler_pointer, char** buffer_ptr, int* binary_index);
AssemblerErrorType ProcessRegisterArgument(Assembler* assembler_pointer, char** buffer_ptr, int* binary_index);
AssemblerErrorType ProcessMemoryArgument  (Assembler* assembler_pointer, char** buffer_ptr, int* binary_index);
AssemblerErrorType ProcessNoArgument      (Assembler* assembler_pointer,                    int* binary_index);
//FIXME

bool CommandRequiresArgument(OpCodes op);
const char* GetAsmErrorString(AssemblerErrorType error);

AssemblerErrorType FirstPass(Assembler* assembler_pointer);
AssemblerErrorType SecondPass(Assembler* assembler_pointer);
AssemblerErrorType WriteBinaryBufferToBinaryFile(Assembler* assembler_pointer, int number_of_ints);

AssemblerErrorType GetInstructionFileFileAndReadItToBuffer(Assembler* assembler_pointer, const char* input_filename);
AssemblerErrorType AssemblerCtor(Assembler* assembler_pointer, const char* input_filename, const char* output_filename);
void AssemblerDtor(Assembler* assembler_pointer);

FILE* GetInputFile(const char* instruction_filename);
FILE* GetOutputFile(const char* output_filename);

RegCodes GetRegisterByName(const char* name);

void InitLabelTable(LabelTable* ptr_table);
int FindLabel(LabelTable* table, const char* name);
AssemblerErrorType AddLabel(LabelTable* table, const char* name, int address);
char* SkipAllSpaceSymbols(char* buffer_ptr);


#endif //MY_ASSEMBLER
