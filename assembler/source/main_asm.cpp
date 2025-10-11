#include <stdio.h>
#include <stdlib.h>

#include "my_assembler.h"

int main(int argc, const char** argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Error opening files in %s\n", argv[0]);
        return 1;
    }
    const char* instruction_filename = argv[1];
    const char* binary_filename = argv[2];

    if (instruction_filename == NULL || binary_filename == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }

    Assembler asm_struct = {};
    AssemblerErrorType error = AssemblerCtor(&asm_struct, instruction_filename, binary_filename);
    if (error != ASM_ERROR_NO)
    {
        fprintf(stderr, "Assembly error: %s\n", GetAsmErrorString(error));
        AssemblerDtor(&asm_struct);
        return 1;
    }

    error = FirstPass(&asm_struct);
    if (error != ASM_ERROR_NO)
    {
        printf("First passing failed: %s\n", GetAsmErrorString(error));
        AssemblerDtor(&asm_struct);
        return 1;
    }

//FIXME - #define ERROR_HANDLER(SecondPass(&asm_struct))
    error = SecondPass(&asm_struct);
    if (error != ASM_ERROR_NO)
    {
        printf("Second passing failed: %s\n", GetAsmErrorString(error));
        AssemblerDtor(&asm_struct);
        return 1;
    }

    AssemblerDtor(&asm_struct);
    return 0;
}
