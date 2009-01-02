#include <stdio.h>

typedef void (*Inst)(void);

static int a = 1;
static int ic = 0;

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
    ic = 0;
    return;
}

int ins[] = { 1,2,4,3,2,3,1,2,4,3,2,3,1,2,2,3,4,4,3,1,4,3,2,1,3,4,2,1,1,4,2,3,5 };
Inst dt[] = { &IOP1, &IOP2, &IOP3, &IOP4, &IOP5 };

int
main(int argc, char *argv[])
{
    int ol;
    Inst f;

    for (ol = 0, f = dt[ins[ic]-1]; ol < 100000000; ++ol) {
        do {
            ic++;
            (*f)();
            f = dt[ins[ic]-1];
        } while (ic != 0);
    }

    printf("%d\n", a);

    return 0;
}
