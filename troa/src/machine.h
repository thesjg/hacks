#ifndef _TROA_MACHINE_H_
#define _TROA_MACHINE_H_

#include "processor.h"


typedef struct troa_machine {
    troa_processor_t *processors;

    // Potentially available memory, used memory
    // Pointers to allocation data/etc.
    // Have each "processor" allocate memory independently from the underlying
    // von-neumann mapping

} troa_machine_t;



#endif /* _TROA_MACHINE_H_ */
