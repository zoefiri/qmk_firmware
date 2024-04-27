#pragma once

typedef struct {
    int x;
    int y;
    int w;
    int h;
} dims;

typedef struct strg {
    char *str;
    int   len;
} strg;

int walk(char *strn);

strg str_wrap(char *strn);
