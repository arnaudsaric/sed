#include "sed.h"

int sed (module* mod, script* scripts, FILE* in, FILE* out, bool quiet) {
    buffers bufs;
    memset(&bufs, 0, sizeof(bufs));
    flags fl;
    if (!scripts->compiled)
        for (script* s = scripts; s; s = s->next) {
            if (s->is_file) {
                FILE* f;
                if (!(f = fopen(s->text, "r"))) {
                    fprintf(stderr, "Could not open script file %s\n", s->text);
                    break;
                }
                else {
                    int i = 0;
                    char big_buffer[BIG_BUFSIZE] = {0};
                    while (fgets(big_buffer + i, BIG_BUFSIZE - i, f))
                        i = strlen(big_buffer);
                    big_buffer[BIG_BUFSIZE-1] = 0;
                    s->tokens = compile(mod->commands, big_buffer);
                }
            }
            else
                s->tokens = compile(mod->commands, s->text);
            if (!s->tokens) {
                fprintf(stderr, "Failed to compile '%s'\n", s->text);
                return EXIT_FAILURE;
            }
            s->mod_data = mod->prepare(s->tokens);
            s->compiled = true;
        }
    int line_number = 0;
    get_new_line(bufs.pattern, in);
    get_new_line(bufs.lookahead, in);
    while(*(bufs.pattern)) {
        memset(&fl, 0, sizeof(fl));
        line_number++;
        memset(bufs.append, 0, BUFSIZE);
        for (script* s = scripts; s; s = s->next) {
            token* tok = s->tokens;
            while (tok) {
                bool matched = mod->match(line_number, bufs.pattern, tok);
                mod->process((unsigned char)tok->command, matched||tok->in_range, &tok, &line_number, &bufs, &fl, s->mod_data, in, out);
                if (fl.nextcycle || fl.stop)
                    break;
                tok = tok->nexttoken;
            }
            if (fl.stop)
                break;
        }
        if (!fl.noprint && !quiet)
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
