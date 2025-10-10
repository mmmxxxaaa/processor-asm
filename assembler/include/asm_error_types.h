#ifndef ASM_ERROR_TYPES_H_
#define ASM_ERROR_TYPES_H_

typedef enum {
    ASM_ERROR_NO                      = 0,
    ASM_ERROR_ALLOCATION_FAILED       = 1 << 1,
    ASM_ERROR_UNKNOWN_COMMAND         = 1 << 2,
    ASM_ERROR_CANNOT_OPEN_INPUT_FILE  = 1 << 3,
    ASM_ERROR_CANNOT_OPEN_OUTPUT_FILE = 1 << 4,
    ASM_ERROR_READING_FILE            = 1 << 5,
    ASM_ERROR_EXPECTED_ARGUMENT       = 1 << 6,
    ASM_ERROR_EXPECTED_REGISTER       = 1 << 7,
    ASM_ERROR_INVALID_REGISTER        = 1 << 8,
    ASM_ERROR_LABEL_TABLE             = 1 << 9,
    ASM_ERROR_UNDEFINED_LABEL         = 1 << 10,
    ASM_ERROR_REDEFINITION_LABEL      = 1 << 11
} AssemblerErrorType;

#endif // ASM_ERROR_TYPES_H_
