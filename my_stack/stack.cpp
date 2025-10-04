#include "stack.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "error_types.h"
#include "my_defense.h"

static const int kCanareika = 25022007;
const int kPoison = 525252;
static const int kGrowDataCoefficient = 2;

#ifdef _DEBUG_CANARY
    #define ON_DEBUG_CANARY(code)  code //не заигрываться с этим
#else
    #define ON_DEBUG_CANARY(code)
#endif //_DEBUG_CANARY

int StackCtor(Stack* stack_pointer, size_t starting_capacity)
{
    assert(stack_pointer);
    assert(starting_capacity > 0);

    stack_pointer -> data = (ElementType*) calloc(starting_capacity + ON_DEBUG_CANARY (2) +0, sizeof(ElementType)); //+0 воспринимается просто как 0
    if (stack_pointer -> data == NULL)
    {
        int errors = ERROR_ALLOCATION_FAILED;
        StackDump(stack_pointer, errors, "Allocating memory in StackCtor failed");
        return errors;
    }

#ifdef _DEBUG_CANARY
    stack_pointer -> data[0] = kCanareika;
    stack_pointer -> data[starting_capacity + 1] = kCanareika;

    for (size_t run_index = 1; run_index <= starting_capacity; run_index++)
        stack_pointer -> data[run_index] = kPoison;

    stack_pointer -> data = stack_pointer -> data + 1;
#else
    for (size_t run_index = 0; run_index < starting_capacity; run_index++)
        stack_pointer -> data[run_index] = kPoison;
#endif // _DEBUG_CANARY
    stack_pointer -> size = 0;
    stack_pointer -> capacity = starting_capacity;

#ifdef _DEBUG_HASH
    size_t starting_hash = CalculateHash(stack_pointer);
    stack_pointer->debug.hash = starting_hash;
#endif //_DEBUG_HASH
    return ERROR_NO;
}

void StackDtor(Stack* stack_pointer)
{
    int errors = StackVerification(stack_pointer);
    if (errors != 0)
    {
        StackDump(stack_pointer, errors, "StackDtor failed");
    }

    if (stack_pointer -> data)
    {
#ifdef _DEBUG_CANARY
        free(stack_pointer -> data - 1);
#else
        free(stack_pointer -> data);
#endif // _DEBUG_CANARY
    }
    stack_pointer -> data = NULL;
    stack_pointer -> size = 0;
    stack_pointer -> capacity = 0;
#if defined(_DEBUG_HASH) || defined(_DEBUG_CANARY)
    stack_pointer -> debug.function_name = NULL;
    stack_pointer -> debug.line = 0;
    stack_pointer -> debug.file_name = 0;
    stack_pointer -> debug.variable_name = 0;
#endif //defined(_DEBUG_HASH) || defined(_DEBUG_CANARY)

#ifdef _DEBUG_HASH
    stack_pointer->debug.hash = 0;
#endif //DEBUG_HASH
}

int StackPush(Stack* stk, ElementType value)
{
    int errors = StackVerification(stk); //ассерт не нужен, т.к. верификатор проверяет указатель
    if (errors != 0)
    {
        StackDump(stk, errors, "StackPush failed");
        return errors;
    }
    if (stk->size == stk->capacity)
    {
        errors = ResizeBuffer(stk);
        if (errors != 0)
        {
            StackDump(stk, errors, "Reallocating in StackPush failed");
            return errors;
        }
    }
    stk -> data[stk->size++] = value;
#ifdef _DEBUG_HASH
    stk -> debug.hash = CalculateHash(stk);
#endif //_DEBUG_HASH
    return ERROR_NO;
}

ElementType StackPop(Stack* stk)
{
    int errors = StackVerification(stk);
    if (errors != 0)
    {
        StackDump(stk, errors, "StackPop failed");
        return kPoison;
    }

    size_t current_size = stk->size;
    if (current_size == 0)
    {
        errors = ERROR_POP_WHEN_SIZE_ZERO;
        StackDump(stk, errors, "There is NO ELEMENT in stack to pop");
        return kPoison;
    }

    ElementType element = stk -> data[current_size - 1];
    stk -> data[current_size - 1] = kPoison;
    stk -> size = current_size - 1;

#ifdef _DEBUG_HASH
    stk -> debug.hash = CalculateHash(stk);
#endif //_DEBUG_HASH
    errors = StackVerification(stk);
    if (errors != 0)
    {
        StackDump(stk, errors, "cannot Pop the element");
        return kPoison;
    }
    return element;
}

void StackDump(const Stack* stk, int errors, const char* msg)
{
    assert(msg != NULL);

    if (stk == NULL)
    {
        printf("stack [%p] %s (", stk, msg);
        return;
    }

    // левая канарейка находится по data[-1], а правая по data[capacity]
    ElementType* real_data_start = stk->data - 1;
    size_t stack_size_only_elements = stk->size;
    size_t stack_capacity = stk->capacity;

    printf("stack [%p] %s (", stk, msg);
    ErrorsParse(errors); //FIXME в процессоре добавить промежуточный обработчик ошибок, чтобы они не сразу печатались, а превращались из енамчика в строку

#if defined(_DEBUG_CANARY) || defined(_DEBUG_HASH)
    printf("Err%d) from %s at %s %d\n", errors, stk->debug.function_name, stk->debug.file_name, stk->debug.line);
#else
    printf("Err%d)\n", errors);
#endif //_DEBUG_CANARY
    printf("    {\n");
    printf("    size = %lu\n", stack_size_only_elements);
    printf("    capacity=%lu\n", stack_capacity);
    printf("    data [%p]\n", real_data_start);
    printf("        {\n");

#ifdef _DEBUG_CANARY
    printf("         [0] = ");
    PrintElement(real_data_start[0]);
    printf(" (LEFT CANAREIKA)\n");
#endif //_DEBUG_CANARY
    for (size_t i = 0; i < stack_size_only_elements; i++)
    {
        printf("        *[%lu] = ", i + 1); // +1 потому что реальные индексы с 1
        PrintElement(stk->data[i]);
        printf("\n");
    }
    for (size_t i = stack_size_only_elements; i < stack_capacity; i++)
    {
        printf("         [%lu] = ", i + 1); // +1 потому что реальные индексы с 1
        PrintElement(stk->data[i]);
        printf(" (POISON)\n");
    }

#ifdef _DEBUG_CANARY
    printf("         [%lu] = ", stack_capacity + 1);
    PrintElement(stk->data[stack_capacity]);
    printf(" (RIGHT CANAREIKA)\n");
#endif //_DEBUG_CANARY
    printf("        }\n");
    printf("}\n");
}

int StackVerification(Stack* stack)
{
    if (stack == NULL)
        return ERROR_NULL_PTR;

    int errors = 0;

#if defined(_DEBUG_HASH) || defined(_DEBUG_CANARY)
    if (stack -> debug.function_name == NULL)
        errors |= ERROR_PTR_FUNCTION_NAME;

    if (stack -> debug.line == 0)
        errors |= ERROR_PTR_NUMBER_LINE;

    if (stack -> debug.file_name == NULL)
        errors |= ERROR_PTR_FILE_NAME;

    if (stack -> debug.variable_name == NULL)
        errors |= ERROR_PTR_VARIABLE_NAME;
#endif //_DEBUG_HASH | _DEBUG_CANARY

#ifdef _DEBUG_CANARY
    if (stack -> data[stack->capacity] != kCanareika)
        errors |= ERROR_RIGHT_CANAREIKA_DAMAGED;

    if (stack -> data[-1] != kCanareika)
        errors |= ERROR_LEFT_CANAREIKA_DAMAGED;
#endif //_DEBUG_CANARY

#ifdef _DEBUG_HASH
    if (!CompareHashOldWithNew(stack))
        errors |= ERROR_HASH_DIFFERENT;
#endif //_DEBUG_HASH

    if (stack -> data == NULL)
    {
        errors |= ERROR_PTR_DATA;
        return errors; //дальше нечего проверять
    }

    if (stack -> capacity == 0)
    {
        errors |= ERROR_CAPACITY_NUMBER;
        return errors; //дальше нечего проверять
    }

    if (stack->size > stack->capacity)
        errors |= ERROR_SIZE_NUMBER;

    return errors;
}

int ErrorsParse(int errors)
{
    if (errors == 0)
        return 0;

    if (errors & ERROR_PTR_FUNCTION_NAME)  printf("ERROR_PTR_FUNCTION_NAME ");
    if (errors & ERROR_PTR_NUMBER_LINE)    printf("ERROR_PTR_NUMBER_LINE ");
    if (errors & ERROR_PTR_FILE_NAME)      printf("ERROR_PTR_FILE_NAME ");
    if (errors & ERROR_PTR_VARIABLE_NAME)  printf("ERROR_PTR_VARIABLE_NAME ");
    if (errors & ERROR_PTR_DATA)           printf("ERROR_PTR_DATA ");
    if (errors & ERROR_SIZE_NUMBER)        printf("ERROR_SIZE_NUMBER ");
    if (errors & ERROR_CAPACITY_NUMBER)    printf("ERROR_CAPACITY_NUMBER ");
    if (errors & ERROR_POP_WHEN_SIZE_ZERO) printf("ERROR_POP_WHEN_SIZE_ZERO ");

    if (errors & ERROR_RIGHT_CANAREIKA_DAMAGED)
        printf("ERROR_RIGHT_CANAREIKA_DAMAGED ");
    if (errors & ERROR_LEFT_CANAREIKA_DAMAGED)
        printf("ERROR_LEFT_CANAREIKA_DAMAGED ");

    if (errors & ERROR_HASH_DIFFERENT)
        printf("ERROR_HASH_DIFFERENT ");
    return 1;
}

int ResizeBuffer(Stack* stk)
{
    size_t new_capacity = (stk->capacity == 0) ? 1 : stk->capacity * kGrowDataCoefficient;

    size_t total_size = (new_capacity + ON_DEBUG_CANARY(2) +0) * sizeof(ElementType);
    ElementType* original_data = stk->data - ON_DEBUG_CANARY(1) +0;
    ElementType* new_memory = (ElementType*) realloc(original_data, total_size);
    if (new_memory == NULL)
        return ERROR_REALLOCATION;

#ifdef _DEBUG_CANARY
    new_memory[0] = kCanareika;
    new_memory[new_capacity + 1] = kCanareika;
#endif //_DEBUG_CANARY

    // ИНИЦИАЛИЗИРУЕМ НОВЫЕ POISONЫ
    for (size_t i = stk->capacity + ON_DEBUG_CANARY(1) +0; i < new_capacity + ON_DEBUG_CANARY(1) +0; i++)
        new_memory[i] = kPoison;

    stk->data = new_memory + ON_DEBUG_CANARY(1) +0;
    stk->capacity = new_capacity;
#ifdef _DEBUG_HASH
    stk -> debug.hash = CalculateHash(stk);
#endif //_DEBUG_HASH
    return 0;
}

void PrintElement(ElementType element)
{
    printf("%d", element);
}

//ДЕЛО СДЕЛАНО ПОБИТОВЫЕ ОПЕРАЦИИ ВМЕСТО +=
//написать мейн, показывающий различные ошибки
//ДЕЛО СДЕЛАНО доделать условную компиляцию канареек
//ДЕЛО СДЕЛАНО сделать хэши
//поменяться кодом с Умаром


//в начале проверяю hash из структуры с тем, который только что пересчитал
//потом уже изменяю hash и помещаю его в структуру
