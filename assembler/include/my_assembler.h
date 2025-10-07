#ifndef MY_ASSEMBLER_H_
#define MY_ASSEMBLER_H_

#include <stdio.h>
#include "asm_error_types.h"

const int kMaxCommandLength = 10;
const int kNRegisters = 8;

typedef enum {
    OP_ERR    = -1,
    OP_HLT    =  0,
    OP_PUSH   =  1,
    OP_POP    =  2,
    OP_ADD    =  3,
    OP_SUB    =  4,
    OP_MUL    =  5,
    OP_DIV    =  6,
    OP_SQRT   =  7,
    OP_OUT    =  8,
    OP_IN     =  9,

    OP_JMP    = 10,
    OP_JB     = 11,
    OP_JBE    = 12,
    OP_JA     = 13,
    OP_JAE    = 14,
    OP_JE     = 15,
    OP_JNE    = 16,

    OP_PUSHR  = 33,
    OP_POPR   = 34,
} OpCodes;

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
    char* instruction_filename; // имя файла на вход и указатель на него
    char* binary_filename; // имя файла на выход и указатель на него
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
