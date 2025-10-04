#include <stdio.h>
#include <stdlib.h>

#include "error_types.h"
#include "stack.h"
#include "my_assembler.h"
#include "processor.h"


// static const size_t kStartingCapacity = 1;

int main()
{
    // INIT(stk1);
    // StackCtor(&stk1, kStartingCapacity);
//1-------------------------------------------------------------------------------------------------
//Попытка вызвать Pop, когда стек пустой + проверка реаллокации
//     StackPush(&stk1, 10);
//     StackPush(&stk1, 20);
//     StackPush(&stk1, 30);
//
//     StackPop(&stk1);
//     StackPop(&stk1);
//     StackPop(&stk1);
//     StackPop(&stk1);
//1-------------------------------------------------------------------------------------------------

    ExecuteBinary("my_binary_file.txt");

//2-------------------------------------------------------------------------------------------------
//Проверка верификатора хэша
//     StackPush(&stk1, 10);
//     StackPush(&stk1, 20);
//
//     stk1.debug.hash = 0;
//     StackPop(&stk1);
//
//2-------------------------------------------------------------------------------------------------
    // StackDtor(&stk1);
    return 0;
}
