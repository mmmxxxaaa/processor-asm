#ifndef MY_STACK_H_
#define MY_STACK_H_

#include <stdlib.h>

#include "stack_error_types.h"

const int kPoison = 525252;

#if defined(DEBUG_CANARY) || defined(DEBUG_HASH)
    #if defined(DEBUG_HASH)
        // Если определен DEBUG_HASH (с хэшем)
        #define INIT(name) Stack name = {{__func__, __LINE__, __FILE__, #name, 0}, NULL, 0, 0}
    #else
        // Если определен только DEBUG_CANARY (без хэша)
        #define INIT(name) Stack name = {{__func__, __LINE__, __FILE__, #name}, NULL, 0, 0}
    #endif
#else
    // Если ни один отладочный макрос не определен
    #define INIT(name) Stack name = {NULL, 0, 0}
#endif

typedef int ElementType;

typedef struct {
    const char* function_name;
    int line;
    const char* file_name;
    const char* variable_name;
#ifdef DEBUG_HASH
    size_t hash;   // NOTE хэш должен быть в стуктуре стека
#endif // DEBUG_HASH
} DebugInfo;

typedef struct {
#if defined(DEBUG_CANARY) || defined(DEBUG_HASH)
    DebugInfo debug;
#endif // DEBUG_CANARY
    ElementType* data;
    size_t size;
    size_t capacity;
} Stack;

int StackCtor(Stack* stack_pointer, size_t starting_capacity);
void StackDtor(Stack* stack_pointer);

int StackVerification(Stack* stack);
int ErrorsParse(int errors);

int StackPUSH(Stack* stk, ElementType value);
ElementType StackPOP(Stack* stk);
void StackDump(const Stack* stk, int errors, const char* msg);
int ResizeBuffer(Stack* stk);

void PrintElement(ElementType element);


#endif // MY_STACK_H_
