#ifndef MY_DEFENSE_H_
#define MY_DEFENSE_H_

#include <stdlib.h>
#include "stack.h"

size_t CalculateHash(const Stack* stack);
int CompareHashOldWithNew(Stack* stack_pointer);


#endif // MY_DEFENSE_H_
