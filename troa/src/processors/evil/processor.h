#ifndef _EVIL_PROCESSOR_H_
#define _EVIL_PROCESSOR_H_


typedef enum {
    EVIL_OP_MOVE,

    EVIL_OP_ADD,
    EVIL_OP_SUBTRACT,
    EVIL_OP_MULTIPLY,
    EVIL_OP_DIVIDE,
    EVIL_OP_MODULUS,
    EVIL_OP_POWER,

    // Perhaps make the base ops capable of vector operations for values
    // contained in a vector or tuple?
    EVIL_OP_VECTOR_ADD,
    EVIL_OP_VECTOR_SUBTRACT,
    EVIL_OP_VECTOR_MULTIPLY,
    EVIL_OP_VECTOR_DIVIDE,
    EVIL_OP_VECTOR_MODULUS,

    EVIL_OP_JUMP
} evil_opcodes;


typedef struct evil_processor {

} evil_processor_t;



#endif /* _EVIL_PROCESSOR_H_ */
