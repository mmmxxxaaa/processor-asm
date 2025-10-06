#ifndef ASM_ERROR_TYPES_H_
#define ASM_ERROR_TYPES_H_

typedef enum {
    ASM_ERROR_NO                      = 0,
    ASM_ERROR_ALLOCATION_FAILED       = 1 << 1,
    ASM_ERROR_REALLOCATION            = 1 << 2,
    ASM_ERROR_NULL_PTR                = 1 << 3,
    ASM_ERROR_UNKNOWN_COMMAND         = 1 << 4,
    ASM_ERROR_CANNOT_OPEN_INPUT_FILE  = 1 << 5,
    ASM_ERROR_CANNOT_OPEN_OUTPUT_FILE = 1 << 6,
    ASM_ERROR_READING_FILE            = 1 << 7,
    ASM_ERROR_EXPECTED_ARGUMENT       = 1 << 8
} AssemblerErrorType;

#endif // ASM_ERROR_TYPES_H_
