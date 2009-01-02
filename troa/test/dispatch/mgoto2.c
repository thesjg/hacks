#include <stdio.h>

typedef void *Inst;

static int a = 1;

int ins[] = { 0,1,3,2,1,2,0,1,3,2,1,2,0,1,1,2,3,3,2,0,3,2,1,0,2,3,1,0,0,3,1,2,4 };
static int ic = 0;

int
main(int argc, char *argv[])
{
    int i, b = 1;
    int ol;
    Inst te;

    Inst dt[] = { &&IOP1, &&IOP2, &&IOP3, &&IOP4, &&IOP5 };
    Inst dt1[] = { &&IOP1, &&IOP2, &&IOP3, &&IOP4, &&IOP5 };
    Inst dt2[] = { &&IOP1, &&IOP2, &&IOP3, &&IOP4, &&IOP5 };
    Inst dt3[] = { &&IOP1, &&IOP2, &&IOP3, &&IOP4, &&IOP5 };
    Inst dt4[] = { &&IOP1, &&IOP2, &&IOP3, &&IOP4, &&IOP5 };

    for (ol = 0; ol < 100000000; ++ol) {
        goto *dt[ins[ic]];
ENDLOOP:
        continue;
    }

    printf("%d\n", a);

    return 0;

IOP1:

    a = a + 1;
    goto *dt1[ins[++ic]];

IOP2:

    a = a - 1;
    goto *dt2[ins[++ic]];

IOP3:

    a = a + 1;
    goto *dt3[ins[++ic]];

IOP4:

    a = a - 1;
    goto *dt4[ins[++ic]];

IOP5:

    ic = 0;
    goto ENDLOOP;

}
