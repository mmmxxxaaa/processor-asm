#ifndef MY_ASSEMBLER_H_
#define MY_ASSEMBLER_H_

#include <stdio.h>
#include "asm_error_types.h"

#include "general_const_and_func.h"

const int kMaxCommandLength = 10;

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
    char* instruction_filename; // имя файла на вход и указатель на него //нельзя конст
    char* binary_filename; // имя файла на выход и указатель на него     //нельзя конст
    FILE* binary_file;
    char* instructions_buffer; // буффер с текстом из входного файла, вводится один раз
    int*  binary_buffer;
} Assembler;

OpCodes GetOpCode(const char* command);
const char* GetAsmErrorString(AssemblerErrorType error);

AssemblerErrorType ReadOpCodesFromInstructionFileAndPutThemToBinaryFile(Assembler* assembler_pointer);
AssemblerErrorType ReadInstructionFileToBuffer(Assembler* assembler_pointer, const char* input_filename);
AssemblerErrorType AssemblerCtor(Assembler* assembler_pointer, const char* input_filename, const char* output_filename);
void AssemblerDtor(Assembler* assembler_pointer);

FILE* GetInputFile(const char* instruction_filename);
FILE* GetOutputFile(const char* output_filename);
long int GetFileSize(FILE* file);

const char* GetRegisterName(RegCodes reg);
RegCodes GetRegisterByName(const char* name);
int IsValidRegister(RegCodes reg);

#endif //MY_ASSEMBLER
