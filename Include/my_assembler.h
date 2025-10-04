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
    const char* instruction_filename; // имя файла на вход и указатель на него
    const char* binary_filename; // имя файла на выход и указатель на него
    FILE*       instruction_file;
    FILE*       binary_file;
    char*       instructions_buffer; // буффер с текстом из входного файла, вводится один раз
    int*        binary_buffer;
} Assembler;
//FIXME написать конструктор и деструктор этой структуры асма

OpCodes GetOpCode(const char* command);
int ReadOpCodesFromInstructionFileAndPutThemToBinaryFile(const char* instruction_filename, const char* binary_filename);
int AssemblerCtor(Assembler* assembler_pointer, const char* input_filename, const char* output_filename, size_t starting_capacity);
void AssemblerDtor(Assembler* assembler_pointer);


#endif //MY_ASSEMBLER
