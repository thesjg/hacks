#include <stdio.h>

static int a = 1;

int ins[] = { 0,1,3,2,1,2,0,1,3,2,1,2,0,1,1,2,3,3,2,0,3,2,1,0,2,3,1,0,0,3,1,2,4 };
static int ic = 0;

int
main(int argc, char *argv[])
{
    int i, b = 1;
    int ol;

    for (ol = 0; ol < 100000000; ++ol) {
        if (ins[ic] == 0)
            goto IOP1;
        else if (ins[ic] == 1)
            goto IOP2;
        else if (ins[ic] == 2)
            goto IOP3;
        else if (ins[ic] == 3)
            goto IOP4;
        else if (ins[ic] == 4)
            goto IOP5;
ENDLOOP:
        continue;
    }

    printf("%d\n", a);

    return 0;

IOP1:

    a = a + 1;

    ic++;
    if (ins[ic] < 2)
        if (ins[ic] == 0)
            goto IOP1;
        else
            goto IOP2;
    else if (ins[ic] < 4)
        if (ins[ic] == 2)
            goto IOP3;
        else
            goto IOP4;
    else
        goto IOP5;

IOP2:

    a = a - 1;

    ic++;
    if (ins[ic] < 2)
        if (ins[ic] == 0)
            goto IOP1;
        else
            goto IOP2;
    else if (ins[ic] < 4)
        if (ins[ic] == 2)
            goto IOP3;
        else
            goto IOP4;
    else
        goto IOP5;

IOP3:

    a = a + 1;

    ic++;
    if (ins[ic] < 2)
        if (ins[ic] == 0)
            goto IOP1;
        else
            goto IOP2;
    else if (ins[ic] < 4)
        if (ins[ic] == 2)
            goto IOP3;
        else
            goto IOP4;
    else
        goto IOP5;

IOP4:

    a = a - 1;

    ic++;
    if (ins[ic] < 2)
        if (ins[ic] == 0)
            goto IOP1;
        else
            goto IOP2;
    else if (ins[ic] < 4)
        if (ins[ic] == 2)
            goto IOP3;
        else
            goto IOP4;
    else
        goto IOP5;

IOP5:

    ic = 0;
    goto ENDLOOP;

}
