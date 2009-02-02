#ifndef _ECACHED_ERROR_H_
#define _ECACHED_ERROR_H_

#include <err.h>
#include <sysexits.h>


#define ecached_err(code, fmt)		\
    do {				\
        err(code, fmt);			\
    } while (0)

#define ecached_errx(code, fmt, ...)	\
    do {				\
        errx(code, fmt, __VA_ARGS__);	\
    } while (0)

#define ecached_warn(fmt)		\
    do {				\
        warn(fmt);			\
    } while (0)


#endif /* _ECACHED_ERROR_H_ */
