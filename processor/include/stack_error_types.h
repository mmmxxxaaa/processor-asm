#ifndef STACK_ERROR_TYPES_H_
#define STACK_ERROR_TYPES_H_

typedef enum {
    ERROR_NO                      = 0,
    ERROR_PTR_FUNCTION_NAME       = 1 << 0,
    ERROR_PTR_NUMBER_LINE         = 1 << 1,
    ERROR_PTR_FILE_NAME           = 1 << 2,
    ERROR_PTR_VARIABLE_NAME       = 1 << 3,
    ERROR_PTR_DATA                = 1 << 4,
    ERROR_SIZE_NUMBER             = 1 << 5,
    ERROR_CAPACITY_NUMBER         = 1 << 6,
    ERROR_POP_WHEN_SIZE_ZERO      = 1 << 7,
    ERROR_RIGHT_CANAREIKA_DAMAGED = 1 << 8,
    ERROR_LEFT_CANAREIKA_DAMAGED  = 1 << 9,
    ERROR_ALLOCATION_FAILED       = 1 << 10,
    ERROR_REALLOCATION            = 1 << 11,
    ERROR_NULL_PTR                = 1 << 12,
    ERROR_HASH_DIFFERENT          = 1 << 13,
    ERR_DIV_BY_0                  = 1 << 14
} ErrorType;

#endif // STACK_ERROR_TYPES_H_
