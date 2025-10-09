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

    char* path_to_instruction_file = AddPrefixWithStrcat(argv[1], kPrefixOfPathToInstructionsFile);
    char* path_to_binary_filename = AddPrefixWithStrcat(argv[2], kPrefixOfPathToBinaryFile);

    if (path_to_instruction_file == NULL || path_to_binary_filename == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(path_to_instruction_file);
        free(path_to_binary_filename);
        return 1;
    }

    Assembler asm_struct = {};
    AssemblerErrorType error = AssemblerCtor(&asm_struct, path_to_instruction_file, path_to_binary_filename);
    if (error != ASM_ERROR_NO)
    {
        fprintf(stderr, "Assembly error: %s\n", GetAsmErrorString(error));
        return 1;
    }
    free(path_to_instruction_file);
    free(path_to_binary_filename);

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
