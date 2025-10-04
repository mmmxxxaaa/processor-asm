#ifdef _DEBUG_HASH
#include "my_defense.h"

#include <assert.h>

#include "stack.h"
//FIXME серьезнее функцию хэша
size_t CalculateHash(const Stack* stack)
{
    assert(stack != NULL);

    size_t sum = 0;

    //Суммируем саму структуру Stack
    unsigned char* stack_bytes = (unsigned char*)stack;

    for (size_t i = 0; i < sizeof(Stack); i++)
        sum += stack_bytes[i];

    size_t current_hash = stack->debug.hash;

    //Вычитаем из хеша сумму байтов поля hash'а
    unsigned char* hash_bytes = (unsigned char*)&current_hash;
    for (size_t i = 0; i < sizeof(current_hash); i++)
        sum -= hash_bytes[i];

    //Суммируем данные в массиве
    if (stack->data && stack->size > 0)
    {
        unsigned char* data_bytes = (unsigned char*)stack->data;
        size_t data_size = stack->size * sizeof(ElementType);

        for (size_t i = 0; i < data_size; i++)
            sum += data_bytes[i];
    }

    return sum;
}

int CompareHashOldWithNew(Stack* stack_pointer)
{
    assert(stack_pointer != NULL);

    size_t new_hash = CalculateHash(stack_pointer);
    size_t old_hash = stack_pointer->debug.hash;
    if (new_hash != old_hash)
        return 0;
    return 1;
}
#endif // _DEBUG_HASH
