#include <stdio.h>
#include <stdlib.h>

#include "my_assembler.h"

#define ERROR_HANDLER(func_call, message)                       \
    do {                                                        \
        error = func_call;                                      \
        if (error != ASM_ERROR_NO) {                            \
            fprintf(stderr, message, GetAsmErrorString(error)); \
            AssemblerDtor(&asm_struct);                         \
            return 1;                                           \
        }                                                       \
    } while(0)

int main(int argc, const char** argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Error in %s. You should enter one filename: name of binary file. \n", argv[0]);
        return 1;
    }
    const char* instruction_filename = argv[1];
    const char* binary_filename = argv[2];
    AssemblerErrorType error = ASM_ERROR_NO;

    Assembler asm_struct = {};
    ERROR_HANDLER(AssemblerCtor(&asm_struct, instruction_filename, binary_filename), "Assembly error: %s\n");

    ERROR_HANDLER(FirstPass(&asm_struct), "First passing failed: %s\n");

    ERROR_HANDLER(SecondPass(&asm_struct), "Second passing failed: %s\n");

    AssemblerDtor(&asm_struct);
    return 0;
}
