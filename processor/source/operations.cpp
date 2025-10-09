#include "operations.h"

#include <stdio.h>
#include <math.h>

int StackADD(Stack* stack)
{
    int errors = StackVerification(stack);
    if (errors != 0)
    {
        StackDump(stack, errors, "StackAdd failed");
        return errors;
    }

    ElementType number1 = StackPOP(stack);
    ElementType number2 = StackPOP(stack);

    ElementType add = number1 + number2;
    StackPUSH(stack, add);

    errors = StackVerification(stack);

    return ERROR_NO;
}

int StackSUB(Stack* stack)
{
    int errors = StackVerification(stack);
    if (errors != 0)
    {
        StackDump(stack, errors, "StackSub failed");
        return errors;
    }

    ElementType number1 = StackPOP(stack);
    ElementType number2 = StackPOP(stack);

    ElementType sub = number2 - number1;
    StackPUSH(stack, sub);

    errors = StackVerification(stack);

    return ERROR_NO;
}
int StackMUL(Stack* stack)
{
    int errors = StackVerification(stack);
    if (errors != 0)
    {
        StackDump(stack, errors, "StackMul failed");
        return errors;
    }

    ElementType number1 = StackPOP(stack);
    ElementType number2 = StackPOP(stack);

    ElementType mul = number1 * number2;
    StackPUSH(stack, mul);

    errors = StackVerification(stack);

    return ERROR_NO;
}
int StackDIV(Stack* stack)
{
    int errors = StackVerification(stack);
    if (errors != 0)
    {
        StackDump(stack, errors, "StackDiv failed");
        return errors;
    }

    ElementType number1 = StackPOP(stack);
    ElementType number2 = StackPOP(stack);

    if (number1 == 0)
    {
        StackPUSH(stack, number2);
        StackPUSH(stack, number1);
        fprintf(stderr, "Division by zero error\n");
        return ERR_DIV_BY_0;
    }
    ElementType div = number2 / number1;
    StackPUSH(stack, div);

    errors = StackVerification(stack);

    return ERROR_NO;
}

int StackSQRT(Stack* stack)
{
    int errors = StackVerification(stack);
    if (errors != 0)
    {
        StackDump(stack, errors, "StackSqrt failed");
        return errors;
    }

    ElementType number = StackPOP(stack);
    number = (ElementType)sqrt(number);
    StackPUSH(stack, number);

    errors = StackVerification(stack);

    return ERROR_NO;
}
