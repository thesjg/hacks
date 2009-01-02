#include <stdio.h>

static int a = 1;

inline void
IOP1(void)
{
    a = a + 1;
}

inline void
IOP2(void)
{
    a = a - 1;
}

inline void
IOP3(void)
{
    a = a + 1;
}

inline void
IOP4(void)
{
    a = a - 1;
}

inline void
IOP5(void)
{
    return;
}


int
main(int argc, char *argv[])
{
    int ol;

    for (ol = 0; ol < 100000000; ++ol) {

        IOP1();
        IOP2();
        IOP4();
        IOP3();
        IOP2();

        IOP3();
        IOP1();
        IOP2();
        IOP4();
        IOP3();

        IOP2();
        IOP3();
        IOP1();
        IOP2();
        IOP2();

        IOP3();
        IOP4();
        IOP4();
        IOP3();
        IOP1();

        IOP4();
        IOP3();
        IOP2();
        IOP1();
        IOP3();

        IOP4();
        IOP2();
        IOP1();
        IOP1();
        IOP4();

        IOP2();
        IOP3();
        IOP5();

    }

    printf("%d\n", a);

    return 0;
}
