#ifndef MY_ASSEMBLER_H_
#define MY_ASSEMBLER_H_

#include <stdio.h>

#define MAX_COMMAND_LENGTH 10
//структура содержит имена и указатели на файлы вход/выход + 2 буфера с входными данными и выходными данными

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
    const char* instruction_filename;
    const char* binary_filename;
    FILE*       instruction_file;
    FILE*       binary_file;
    char*       instructions_buffer;
} Assembler;


OpCodes GetOpCode(const char* command);
int ReadOpCodesFromInstructionFileAndPutThemToBinaryFile(const char* instruction_filename, const char* binary_filename);

#endif //MY_ASSEMBLER
