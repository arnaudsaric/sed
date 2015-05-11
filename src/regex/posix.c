#define _POSIX_C_SOURCE 200112L

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

typedef unsigned char bool;

int compile(void** regex, const char* definition, bool justmatch) {
    regex_t *preg = malloc(sizeof(regex_t));
    int ret = regcomp(preg, definition, REG_NEWLINE | (justmatch*REG_NOSUB));
    *regex = preg;
    return ret;
}

bool match(const void* regex, const char* str) {
    return (regexec((regex_t*)regex, str, 0, 0, 0) == 0);
}

bool exec(const void* regex, const char* str, int n_matches, int* starts, int* ends) {
    regmatch_t pmatch[n_matches+1];
    bool ret = regexec((regex_t*) regex, str, n_matches, pmatch, 0);
    for (int i = 0; i <= n_matches; i++) {
        starts[i] = pmatch[i].rm_so;
        ends[i] = pmatch[i].rm_eo;
    }
    return ret;
}

void rm(void* regex) {
    regfree((regex_t*) regex);
}
