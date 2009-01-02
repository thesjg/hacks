#include <stdio.h>

typedef void (*Inst)(void);

static int a = 1;
static int ic = 0;

int ins[] = { 1,2,4,3,2,3,1,2,4,3,2,3,1,2,2,3,4,4,3,1,4,3,2,1,3,4,2,1,1,4,2,3,5 };

void IOP1(void);
void IOP2(void);
void IOP3(void);
void IOP4(void);
void IOP5(void);
Inst dt[] = { &IOP1, &IOP2, &IOP3, &IOP4, &IOP5 };

void
IOP1(void)
{
    Inst f = dt[ins[++ic]-1];

    a = a + 1;

    (*f)();
}

void
IOP2(void)
{
    Inst f = dt[ins[++ic]-1];

    a = a - 1;

    (*f)();
}

void
IOP3(void)
{
    Inst f = dt[ins[++ic]-1];

    a = a + 1;

    (*f)();
}

void
IOP4(void)
{
    Inst f = dt[ins[++ic]-1];

    a = a - 1;

    (*f)();
}

void
IOP5(void)
{
    ic = 0;
    return;
}

int
main(int argc, char *argv[])
{
    int ol;

    for (ol = 0; ol < 100000000; ++ol) {
        (*dt[ins[ic]-1])();
    }

    printf("%d\n", a);

    return 0;
}
