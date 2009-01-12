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
    size_t last = nlen - 1;
    u_char s_mask = UCHAR_MAX;

    if (nlen <= 0 || !haystack || !needle)
        return -1;
 
    for (scan = 0; scan <= UCHAR_MAX; ++scan)
        bad_char_skip[scan] = nlen;
  
    for (scan = 0; scan < last; ++scan)
        bad_char_skip[needle[scan]] = last - scan;

    for (scan = 0; scan < nlen; ++scan)
        s_mask &= needle[scan];

    if (s_mask != 0)
        do {
            if ((s_mask & haystack[scan]) != s_mask) {
                t_hlen   -= nlen;
                haystack += nlen;
                continue;
            }

            for (scan = last; haystack[scan] == needle[scan]; --scan)
                if (scan == 0) /* If the first byte matches, we've found it. */
                    return (hlen - t_hlen);
 
            t_hlen   -= bad_char_skip[haystack[last]];
            haystack += bad_char_skip[haystack[last]];
        } while (t_hlen >= nlen);
    else
        do {
            for (scan = last; haystack[scan] == needle[scan]; --scan)
                if (scan == 0)
                    return (hlen - t_hlen);

            t_hlen   -= bad_char_skip[haystack[last]];
            haystack += bad_char_skip[haystack[last]];
        } while (t_hlen >= nlen);

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
