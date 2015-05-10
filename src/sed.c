#include "util.h"
#include "compile.h"
#include "module.h"

int sed (module* mod, char* commands, FILE* in, FILE* out) {
    buffers bufs;
    memset(&bufs, 0, sizeof(bufs));
    flags fl;
    token* first = compile(mod->commands, commands);
    if (!first) {
        fprintf(stderr, "Failed to compile '%s'\n", commands);
        return EXIT_FAILURE;
    }
    void* mod_data = mod->prepare(first);
    int line_number = 0;
    get_new_line(bufs.pattern, in);
    get_new_line(bufs.lookahead, in);
    while(*(bufs.pattern)) {
        token* tok = first;
        memset(&fl, 0, sizeof(fl));
        line_number++;
        memset(bufs.append, 0, BUFSIZE);
        while (tok) {
            bool matched = mod->match(line_number, bufs.pattern, &(tok->in_range), tok->addresses, tok->subtypes, tok->delimiter);
            mod->process((unsigned char)tok->command, matched||tok->in_range, &tok, &line_number, &bufs, &fl, mod_data, in, out);
            if (fl.nextcycle || fl.stop)
                break;
            tok = tok->nexttoken;
        }
        if (!fl.noprint)
            printf("%s\n", bufs.pattern);
        if (*(bufs.append))
            printf("%s\n", bufs.append);
        if (fl.stop)
            break;
        if (!fl.noread) {
            strcpy(bufs.pattern, (const char*)bufs.lookahead);
            get_new_line(bufs.lookahead, in);
        }
    }
    return 0;
}

void usage(char* invocation) {
    fprintf(stderr, "Usage: %s 'commands' file\n", invocation);
    exit(EXIT_FAILURE);
}

int main (int argc, char ** argv) {
    if (argc == 1)
        usage(argv[0]);
    module* mod_posix;
    init_mod(&mod_posix,"modules/posix.so");
    if (argc == 2)
        exit(sed(mod_posix, argv[1], stdin, stdout));
    FILE *file = 0;
    int ret = 0;
    for (int i = 3; i <= argc; i++)
        if (!(file = fopen(argv[i-1], "r")))
            fprintf(stderr, "Could not open %s\n", argv[i-1]);
        else
            ret += sed(mod_posix, argv[1], file, stdout);
    rm_mod(mod_posix);
    exit(ret);
}
