%{

#include <stdio.h>

#include <sys/types.h>


u_int32_t line_number = 0;

%}

whitespace	[ \w]+
newline		\r\n|\r|\n

%%

{whitespace}
{newline}	line_number++;

%%

int
main(int argc, char *argv[])
{
    yylex();

    printf("lines: %d\n", line_number);

    return 0;
}
