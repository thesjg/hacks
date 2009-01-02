#include <stdio.h>

typedef void *Inst;

static int a = 1;

int ins[] = { 1,2,4,3,2,3,1,2,4,3,2,3,1,2,2,3,4,4,3,1,4,3,2,1,3,4,2,1,1,4,2,3,5 };
static int ic = 0;

int
main(int argc, char *argv[])
{
    int i, b = 1;
    int ol;
    Inst te;

    Inst dt[] = { &&IOP1, &&IOP2, &&IOP3, &&IOP4, &&IOP5 };

    for (ol = 0; ol < 100000000; ++ol) {
        te = dt[ins[ic]-1];
        goto *te;
ENDLOOP:
        continue;
    }

    printf("%d\n", a);

    return 0;

IOP1:

    a = a + 1;

    ic++;
    te = dt[ins[ic]-1];
    goto *te;

IOP2:

    a = a - 1;

    ic++;
    te = dt[ins[ic]-1];
    goto *te;

IOP3:

    a = a + 1;

    ic++;
    te = dt[ins[ic]-1];
    goto *te;

IOP4:

    a = a - 1;

    ic++;
    te = dt[ins[ic]-1];
    goto *te;

IOP5:

    ic = 0;
    goto ENDLOOP;

}
