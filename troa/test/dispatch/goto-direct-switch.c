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
        switch (ins[ic]) {
            case 0:
                goto IOP1;
            case 1:
                goto IOP2;
            case 2:
                goto IOP3;
            case 3:
                goto IOP4;
            case 4:
                goto IOP5;
        }
ENDLOOP:
        continue;
    }

    printf("%d\n", a);

    return 0;

IOP1:

    a = a + 1;

    ic++;
    switch (ins[ic]) {
        case 0:
            goto IOP1;
        case 1:
            goto IOP2;
        case 2:
            goto IOP3;
        case 3:
            goto IOP4;
        case 4:
            goto IOP5;
    }

IOP2:

    a = a - 1;

    ic++;
    switch (ins[ic]) {
        case 0:
            goto IOP1;
        case 1:
            goto IOP2;
        case 2:
            goto IOP3;
        case 3:
            goto IOP4;
        case 4:
            goto IOP5;
    }

IOP3:

    a = a + 1;

    ic++;
    switch (ins[ic]) {
        case 0:
            goto IOP1;
        case 1:
            goto IOP2;
        case 2:
            goto IOP3;
        case 3:
            goto IOP4;
        case 4:
            goto IOP5;
    }

IOP4:

    a = a - 1;

    ic++;
    switch (ins[ic]) {
        case 0:
            goto IOP1;
        case 1:
            goto IOP2;
        case 2:
            goto IOP3;
        case 3:
            goto IOP4;
        case 4:
            goto IOP5;
    }

IOP5:

    ic = 0;
    goto ENDLOOP;

}
