#ifndef GENERAL_CONST_AND_FUNC_H_
#define GENERAL_CONST_AND_FUNC_H_

#include <stdio.h>

#define FREE_AND_NULL(ptr)      \
    do {                        \
        free(ptr);              \
        (ptr) = NULL;           \
    } while(0)

static const int kNRegisters = 8;

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

    OP_CALL   = 17,
    OP_RET    = 18,

    OP_PUSHM  = 19,
    OP_POPM   = 20,
    OP_DRAW   = 21,

    OP_PUSHR  = 22,
    OP_POPR   = 23
} OpCodes;

long int GetFileSize(FILE* file);


#endif // GENERAL_CONST_AND_FUNC_H_
