#include <stdio.h>

static int a = 1;

void
IOP1(void)
{
    a = a + 1;
}

void
IOP2(void)
{
    a = a - 1;
}

void
IOP3(void)
{
    a = a + 1;
}

void
IOP4(void)
{
    a = a - 1;
}

void
IOP5(void)
{
    return;
}


int
main(int argc, char *argv[])
{
    int ol;

#define xCall(f) __asm("call " #f)

    for (ol = 0; ol < 100000000; ++ol) {

        xCall(IOP1);
        xCall(IOP2);
        xCall(IOP4);
        xCall(IOP3);
        xCall(IOP2);

        xCall(IOP3);
        xCall(IOP1);
        xCall(IOP2);
        xCall(IOP4);
        xCall(IOP3);

        xCall(IOP2);
        xCall(IOP3);
        xCall(IOP1);
        xCall(IOP2);
        xCall(IOP2);

        xCall(IOP3);
        xCall(IOP4);
        xCall(IOP4);
        xCall(IOP3);
        xCall(IOP1);

        xCall(IOP4);
        xCall(IOP3);
        xCall(IOP2);
        xCall(IOP1);
        xCall(IOP3);

        xCall(IOP4);
        xCall(IOP2);
        xCall(IOP1);
        xCall(IOP1);
        xCall(IOP4);

        xCall(IOP2);
        xCall(IOP3);
        xCall(IOP5);

    }

    printf("%d\n", a);

    return 0;
}
