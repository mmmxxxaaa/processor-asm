#include <stdio.h>
#include <stdlib.h>

// #include "stack_error_types.h"
// #include "stack.h"
#include "my_assembler.h"
// #include "processor.h"

static const int kStartingCapacityForAssembler = 800;

int main()
{
    Assembler asm_struct = {};
    AssemblerErrorType error = AssemblerCtor(&asm_struct, "../my_text_instructions.txt", "../my_binary_file.txt", kStartingCapacityForAssembler); //FIXME через аргументы командной строки
    if (error != ASM_ERROR_NO)
    {
        fprintf(stderr, "Assembly error: %s\n", GetAsmErrorString(error));
        return 1;
    }

    error = ReadOpCodesFromInstructionFileAndPutThemToBinaryFile(&asm_struct);
    if (error != ASM_ERROR_NO)
    {
        fprintf(stderr, "Assembly error: %s\n", GetAsmErrorString(error));
        AssemblerDtor(&asm_struct);
        return 1;
    }

    AssemblerDtor(&asm_struct);
    return 0;
}
