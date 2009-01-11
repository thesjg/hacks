#include <sys/types.h>

#include <stdio.h>
#include <string.h>
#include <limits.h>

 
const size_t
bhs(const u_char *haystack, size_t hlen,
    const u_char *needle, size_t nlen)
{
    size_t scan = 0;
    size_t bad_char_skip[UCHAR_MAX + 1];
    size_t t_hlen = hlen;

    if (nlen <= 0 || !haystack || !needle)
        return -1;
 
    for (scan = 0; scan <= UCHAR_MAX; scan = scan + 1)
        bad_char_skip[scan] = nlen;
 
    size_t last = nlen - 1;
 
    for (scan = 0; scan < last; scan = scan + 1)
        bad_char_skip[needle[scan]] = last - scan;
 
    while (t_hlen >= nlen)
    {
        for (scan = last; haystack[scan] == needle[scan]; scan = scan - 1)
            if (scan == 0) /* If the first byte matches, we've found it. */
                return (hlen - t_hlen);
 
        t_hlen     -= bad_char_skip[haystack[last]];
        haystack += bad_char_skip[haystack[last]];
    }
 
    return -1;
}

int
main(int argc, char *argv[])
{
    char *n;
    const char h[] = "afeedleyabbadabbadumhahahahathisiswayfunandilikecookiesfumfumfoofumfee";
    int i, s;
    size_t j;

    n = "foo";
    s = strlen(n);
    for (i = 0; i < 10000000; ++i) {
        j = bhs(h, 70, n, s);
    }
    printf("%i\n", j);

    n = "thisiswayfun";
    s = strlen(n);
    for (i = 0; i < 10000000; ++i) {
        j = bhs(h, 70, n, s);
    }
    printf("%i\n", j);

    n = "adumha";
    s = strlen(n);
    for (i = 0; i < 10000000; ++i) {
        j = bhs(h, 70, n, s);
    }
    printf("%i\n", j);

    return 0;
}
