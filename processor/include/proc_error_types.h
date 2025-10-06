#ifndef PROC_ERROR_TYPES_H_
#define PROC_ERROR_TYPES_H_

typedef enum {
    PROC_ERROR_NO                      = 0,
    PROC_ERROR_ALLOCATION_FAILED       = 1 << 1,
    PROC_ERROR_CANNOT_OPEN_BINARY_FILE = 1 << 2,
    PROC_ERROR_READING_FILE            = 1 << 3,
    PROC_ERROR_UNKNOWN_OPCODE          = 1 << 4,
    PROC_ERROR_STACK_OPERATION_FAILED  = 1 << 5,
    PROC_ERROR_INVALID_STATE           = 1 << 6
} ProcessorErrorType;

#endif // PROC_ERROR_TYPES_H_
