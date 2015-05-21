#include "compile.h"

int read_pattern(const char* description, char nextchar, char* in, char*** out, int* n_out) {
    int ctr = 0;
    char buffer[BUFSIZE], delim[8], *pattern = (char*)description;
    memset(delim, 0, 8);
    while (*pattern && *in) {
        if (*pattern != '%' && *pattern == *in) {
            pattern++;
            ctr++;
            in++;
            continue;
        }
        assert((*pattern == '%'), "Pattern violation\n", -1);
        pattern++;
        if (*pattern == '%' && *in == '%') {
            pattern++;
            ctr++;
            in++;
            continue;
        }
        bool optional = (*pattern == '?'), double_delim = false;
        int i = 0;
        char this_delim = 0;
        if (optional)
            pattern++;
        memset(buffer, 0, BUFSIZE);
        switch(*pattern) {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                if (!delim[*pattern - 0x31])
                    delim[*pattern - 0x31] = *in;
                else
                    assert((delim[*pattern - 0x31] == *in), "Pattern violation\n", -1);
                pattern++;
                ctr++;
                in++;
                break;
            case 'l':
                if (*in == '$') {
                    *out = (char**) realloc(*out, (++(*n_out)) * sizeof(char*));
                    (*out)[*n_out-1] = (char*) malloc(2);
                    (*out)[*n_out-1][0] = '$';
                    (*out)[*n_out-1][1] = 0;
                    pattern++;
                    ctr++;
                    in++;
                    break;
                }
            case 'd':
                pattern++;
                if (!optional) {
                    assert ((*in >= '0' && *in <= '9'), "Pattern violation\n", -1);
                }
                else if (*in < '0' || *in > '9')
                    break;
                fill_step(buffer);
                fill_while(buffer, *in >= '0' && *in <= '9');
                buffer[i] = 0;
                ctr += i;
                *out = (char**) realloc(*out, (++(*n_out)) * sizeof(char*));
                (*out)[*n_out-1] = (char*) malloc(strlen(buffer));
                strcpy((*out)[*n_out-1], (const char*) buffer);
                break;
            case 's':
                pattern++;
                bool stop_at_delim = false;
                if (!*pattern) {
                    this_delim = nextchar;
                    stop_at_delim = true;
                }
                else if (*pattern != '%')
                    this_delim = *pattern;
                else {
                    pattern++;
                    if (*pattern == '%')
                        this_delim = '%';
                    else if (*pattern > '0' && *pattern < '9') {
                        assert(delim[*pattern - 0x31], "Use of delimiter bedore definition\n", -1);
                        this_delim = delim[*pattern - 0x31];
                    }
                    else
                        assert(false, "Invalid delimiter\n", -1);
                    double_delim = true;
                }
                if (!optional) {
                    assert ((*in != this_delim), "Pattern violation\n", -1);
                }
                else if (*in == this_delim) {
                    if (double_delim)
                        pattern--;
                    break;
                }
                pattern++;
                fill_step(buffer);
                fill_while(buffer, *in != this_delim);
                buffer[i] = 0;
                ctr += i;
                if (!stop_at_delim) {
                    in++;
                    ctr++;
                }
                *out = (char**) realloc(*out, (++(*n_out)) * sizeof(char*));
                (*out)[*n_out-1] = (char*) malloc(strlen(buffer));
                strcpy((*out)[*n_out-1], (const char*) buffer);
        }
    }
    return ctr;
}

token* compile(const command **valid_commands, char* commands) {
    token *first = (token*) malloc(sizeof(token)), *tok = first;
    memset(first, 0, sizeof(token));
    while (*commands) {
        if (*commands == ' ' || *commands == '\t' || *commands == '\n') {
            commands++;
            continue;
        }
        if (*commands == ';') {
            tok->nexttoken = (token*) malloc(sizeof(token));
            tok = tok->nexttoken;
            memset(tok, 0, sizeof(token));
            commands++;
            continue;
        }
        if (*commands == '!') {
            assert(!tok->command, "Can't have negation after command\n", NULL)
            tok->address_negate = true;
            commands++;
            continue;
        }
        const command* valid_command = valid_commands[(unsigned char)*commands];
        assert(valid_command, "Unknown command\n", NULL);
        if (valid_command->type == DELIMITER) {
            assert(tok->n_addresses, "Can't have a delimiter before first address\n", NULL);
            tok->delimiter = *commands;
            commands++;
        }
        else if (valid_command->type == ADDRESS) {
            assert((tok->n_addresses <= valid_command->allow_address), "Too many addresses\n", NULL);
            tok->subtypes = realloc(tok->subtypes, (1+tok->n_addresses) * sizeof(int));
            tok->subtypes[tok->n_addresses] = valid_command->subtype;
            int ret = read_pattern(valid_command->description, valid_command->nextchar, commands, &(tok->addresses), &(tok->n_addresses));
            assert(ret, "Null pattern?\n", NULL);
            commands += ret;
        }
        else {
            assert((tok->n_addresses <= valid_command->allow_address), "Too many addresses\n", NULL);
            tok->command = *commands;
            int ret = read_pattern(valid_command->description, valid_command->nextchar, commands, &(tok->args), &(tok->n_args));
            assert(ret, "Null pattern?\n", NULL);
            commands += ret;
        }
    }
    return first;
}
