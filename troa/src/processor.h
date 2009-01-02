#ifndef _TROA_PROCESSOR_H_
#define _TROA_PROCESSOR_H_


typedef enum {
    TROA_OP_MOVE,

    TROA_OP_ADD,
    TROA_OP_SUBTRACT,
    TROA_OP_MULTIPLY,
    TROA_OP_DIVIDE,
    TROA_OP_MODULUS,
    TROA_OP_POWER,

    // Perhaps make the base ops capable of vector operations for values
    // contained in a vector or tuple?
    TROA_OP_VECTOR_ADD,
    TROA_OP_VECTOR_SUBTRACT,
    TROA_OP_VECTOR_MULTIPLY,
    TROA_OP_VECTOR_DIVIDE,
    TROA_OP_VECTOR_MODULUS,

    TROA_OP_JUMP
} troa_opcodes;


typedef struct troa_processor {

} troa_processor_t;



#endif /* _TROA_PROCESSOR_H_ */
