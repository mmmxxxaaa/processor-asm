#ifndef MY_ASSEMBLER_H_
#define MY_ASSEMBLER_H_

#include <stdio.h>
#include "asm_error_types.h"

#define MAX_COMMAND_LENGTH 10 //FIXME в константу

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
    OP_OUT    =  8
} OpCodes;

typedef struct {
    char* instruction_filename; // имя файла на вход и указатель на него
    char* binary_filename; // имя файла на выход и указатель на него
    FILE*       binary_file;
    char*       instructions_buffer; // буффер с текстом из входного файла, вводится один раз
    int*        binary_buffer;
} Assembler;
//ДЕЛО СДЕЛАНО написать конструктор и деструктор этой структуры асма

OpCodes GetOpCode(const char* command);
const char* GetAsmErrorString(AssemblerErrorType error);

AssemblerErrorType ReadOpCodesFromInstructionFileAndPutThemToBinaryFile(Assembler* assembler_pointer);
AssemblerErrorType ReadInstructionFileToBuffer(Assembler* assembler_pointer, const char* input_filename);
AssemblerErrorType AssemblerCtor(Assembler* assembler_pointer, const char* input_filename, const char* output_filename, size_t starting_capacity);
void AssemblerDtor(Assembler* assembler_pointer);

FILE* GetInputFile(const char* instruction_filename);
FILE* GetOutputFile(const char* output_filename);
long int GetFileSize(FILE* file);

#endif //MY_ASSEMBLER
