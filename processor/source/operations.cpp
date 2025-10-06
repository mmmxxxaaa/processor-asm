#include "operations.h"

#include <stdio.h>
#include <math.h>

int StackAdd(Stack* stack)
{
    int errors = StackVerification(stack);
    if (errors != 0)
    {
        StackDump(stack, errors, "StackAdd failed");
        return errors;
    }

    ElementType number1 = StackPop(stack);
    ElementType number2 = StackPop(stack);

    ElementType add = number1 + number2;
    StackPush(stack, add);

    errors = StackVerification(stack);

    return ERROR_NO;
}

int StackSub(Stack* stack)
{
    int errors = StackVerification(stack);
    if (errors != 0)
    {
        StackDump(stack, errors, "StackSub failed");
        return errors;
    }

    ElementType number1 = StackPop(stack);
    ElementType number2 = StackPop(stack);

    ElementType sub = number2 - number1;
    StackPush(stack, sub);

    errors = StackVerification(stack);

    return ERROR_NO;
}
int StackMul(Stack* stack)
{
    int errors = StackVerification(stack);
    if (errors != 0)
    {
        StackDump(stack, errors, "StackMul failed");
        return errors;
    }

    ElementType number1 = StackPop(stack);
    ElementType number2 = StackPop(stack);

    ElementType mul = number1 * number2;
    StackPush(stack, mul);

    errors = StackVerification(stack);

    return ERROR_NO;
}
int StackDiv(Stack* stack)
{
    int errors = StackVerification(stack);
    if (errors != 0)
    {
        StackDump(stack, errors, "StackDiv failed");
        return errors;
    }

    ElementType number1 = StackPop(stack);
    ElementType number2 = StackPop(stack);

    if (number1 == 0)
    {
        StackPush(stack, number2);
        StackPush(stack, number1);
        fprintf(stderr, "Division by zero error\n");
        return ERR_DIV_BY_0;
    }
    ElementType div = number2 / number1;
    StackPush(stack, div);

    errors = StackVerification(stack);

    return ERROR_NO;
}

int StackSqrt(Stack* stack)
{
    int errors = StackVerification(stack);
    if (errors != 0)
    {
        StackDump(stack, errors, "StackSqrt failed");
        return errors;
    }

    ElementType number = StackPop(stack);
    number = (ElementType)sqrt(number);
    StackPush(stack, number);

    errors = StackVerification(stack);

    return ERROR_NO;
}
