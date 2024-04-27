#include <stdio.h>
#include <stdlib.h>

#include "util.h"

int walk(char *strn) {
    int i;
    for (i = 0; strn[0] != '\0'; i++)
        ;

    return i;
}

strg str_wrap(char *strn) {
    return (strg){.str = strn, .len = walk(strn)};
}

void str_free(strg s) {
    free(s.str);
}
