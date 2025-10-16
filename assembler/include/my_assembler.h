#ifndef MY_ASSEMBLER_H_
#define MY_ASSEMBLER_H_

#include <stdio.h>
#include "asm_error_types.h"

#include "general_const_and_func.h"

#define COMPARE_COMMAND(cmd, name) if (strcmp(command, #name) == 0) return OP_##name

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

typedef struct {
    char name[kMaxLabelLength];
    int address;
} Label;

typedef struct {
    Label labels[kMaxNOfLabels];
    int number_of_labels;
} LabelTable;

typedef struct {
    const char* instruction_filename; // имя файла на вход и указатель на него //нельзя конст
    const char* binary_filename; // имя файла на выход и указатель на него     //нельзя конст
    FILE* binary_file;
    char* instructions_buffer; // буффер с текстом из входного файла, вводится один раз
    int*  binary_buffer;
    int   size_of_binary_file;
    LabelTable label_table;
} Assembler;

OpCodes GetOpCode(const char* command);
const char* GetAsmErrorString(AssemblerErrorType error);

AssemblerErrorType FirstPass(Assembler* assembler_pointer);
int CommandRequiresArgument(OpCodes op);
AssemblerErrorType SecondPass(Assembler* assembler_pointer);

AssemblerErrorType ReadInstructionFileToBuffer(Assembler* assembler_pointer, const char* input_filename);
AssemblerErrorType AssemblerCtor(Assembler* assembler_pointer, const char* input_filename, const char* output_filename);
void AssemblerDtor(Assembler* assembler_pointer);

FILE* GetInputFile(const char* instruction_filename);
FILE* GetOutputFile(const char* output_filename);
long int GetFileSize(FILE* file);

RegCodes GetRegisterByName(const char* name);


void InitLabelTable(LabelTable* ptr_table);
int FindLabel(LabelTable* table, const char* name);
AssemblerErrorType AddLabel(LabelTable* table, const char* name, int address);
void SkipAllSpaceSymbols(char** ptr_to_buffer_ptr);


#endif //MY_ASSEMBLER
