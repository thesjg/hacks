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


int ins[] = { 1,2,4,3,2,3,1,2,4,3,2,3,1,2,2,3,4,4,3,1,4,3,2,1,3,4,2,1,1,4,2,3,5 };

int
main(int argc, char *argv[])
{
    int ol, il;

#define xCall(f) __asm("call " #f)

    for (ol = 0; ol < 100000000; ++ol) {
        for (il = 0; il < sizeof(ins) / sizeof(int); ++il) {
            switch (ins[il]) {
                case 1:
                    xCall(IOP1);
                    break;
                case 2:
                    xCall(IOP2);
                    break;
                case 3:
                    xCall(IOP3);
                    break;
                case 4:
                    xCall(IOP4);
                    break;
                case 5:
                    xCall(IOP5);
                    break;
            }
        }
    }

    printf("%d\n", a);

    return 0;
}
