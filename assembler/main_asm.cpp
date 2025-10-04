#include <stdio.h>
#include <stdlib.h>

#include "stack_error_types.h"
#include "stack.h"
#include "my_assembler.h"
#include "processor.h"

int main()
{
    ReadOpCodesFromInstructionFileAndPutThemToBinaryFile("my_text_instructions.txt", "my_binary_file.txt"); //FIXME через аргументы командной строки
    return 0;
}
