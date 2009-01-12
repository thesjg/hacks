#include <stdio.h>

typedef void *Inst;

static int a = 1;
static int ic = 0;

inline void
OP1()
{
    a = a + 1;
}

inline void
OP2()
{
    a = a - 1;
}

inline void
OP3()
{
    a = a + 1;
}

inline void
OP4()
{
    a = a - 1;
}


int
main(int argc, char *argv[])
{
    int i, b = 1;
    int ol;
    Inst te;

    Inst ins[] = { &&IOP1_2, &&IOP2_4, &&IOP4_3, &&IOP3_2, &&IOP2_3, &&IOP3_1,
                   &&IOP1_2, &&IOP2_4, &&IOP4_3, &&IOP3_2, &&IOP2_3, &&IOP3_1,
                   &&IOP1_2, &&IOP2_2, &&IOP2_3, &&IOP3_4, &&IOP4_4, &&IOP4_3,
                   &&IOP3_1, &&IOP1_4, &&IOP4_3, &&IOP3_2, &&IOP2_1, &&IOP1_3,
                   &&IOP3_4, &&IOP4_2, &&IOP2_1, &&IOP1_1, &&IOP1_4, &&IOP4_2,
                   &&IOP2_3, &&IOP3_5 };


    for (ol = 0; ol < 100000000; ++ol) {
        goto *ins[ic];
ENDLOOP:
        continue;
    }

    printf("%d\n", a);

    return 0;

IOP1_1:
    OP1();
    goto *ins[++ic];
IOP1_2:
    OP1();
    goto *ins[++ic];
IOP1_3:
    OP1();
    goto *ins[++ic];
IOP1_4:
    OP1();
    goto *ins[++ic];

IOP2_1:
    OP2();
    goto *ins[++ic];
IOP2_2:
    OP2();
    goto *ins[++ic];
IOP2_3:
    OP2();
    goto *ins[++ic];
IOP2_4:
    OP2();
    goto *ins[++ic];

IOP3_1:
    OP3();
    goto *ins[++ic];
IOP3_2:
    OP3();
    goto *ins[++ic];
IOP3_4:
    OP3();
    goto *ins[++ic];
IOP3_5:
    OP3();
    goto IOP5;

IOP4_2:
    OP4();
    goto *ins[++ic];
IOP4_3:
    OP4();
    goto *ins[++ic];
IOP4_4:
    OP4();
    goto *ins[++ic];

IOP5:
    ic = 0;
    goto ENDLOOP;

}
