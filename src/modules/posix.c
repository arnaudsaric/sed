#include "module.h"

void init_mod_posix(module** mod);

#define subtype_LINE 0
#define subtype_PATTERN 1
#define MAXLABELS 256

typedef struct {
    char* key;
    token* target;
} label;

int flag_ignore;
process_func dispatch_array[128];

#include "posix_process.c"

void print(bool printall) {
    if (printall) {
        printf("Commands for module POSIX:\n");
        printf("\n");
        printf("General syntax is [address] command [args] ;\n");
        printf("\n");
    }
    printf("Addresses:\n");
    printf("Valid addresses are:\n");
    printf("\t- A number.    This address will be matched when the corresponding line number of input is read.\n");
    printf("\t- $            This address will be matched on the last line of input.\n");
    printf("\t- /regex/      This address will be matched on every line of input which matches the regex.\n");
    printf("\t- \\cregexc    Same, but with any delimiter.\n");
    printf("Valid address ranges are:\n");
    printf("\t- addr1,addr2  This range will match as soon as addr1 matches, until addr2 matches. It will match at least once.\n");
    printf("\n");
    printf("Commands:\n");
    printf("The number of addresses given here is the maximum accepted, you can always give less than this number (even 0).\n");
    printf("No-address commands:\n");
    printf("\t- :label    Defines a label to which b and t can branch. Does nothing.\n");
    printf("\t- }         Delimits a number of commands with {. Does nothing.\n");
    printf("\t- #comment  Is a comment. Does nothing.\n");
    printf("1-address commands:\n");
    printf("\t- atext     Appends some text. The text will be printed at the end of the cycle.\n");
    printf("\t- itext     Inserts some text. The text will be printed immediately.\n");
    printf("\t- q         Quits. Unless -n option is set, current pattern space will still be output.\n");
    printf("\t- rfile     Reads file. Outputs the content of the file on the output.\n");
    printf("\t- =         Prints current line number, followed by a newline.\n");
    printf("2-address commands:\n");
    printf("\t- {         If the address does not match, ignores the commands until the corresponding }.\n");
    printf("\t- blabel    Branches to label. If no label is given, branch to the end of the script.\n");
    printf("\t- ctext     Changes pattern space to text. Restart a new cycle.\n");
    printf("\t- d         Deletes pattern space and start a new cycle.\n");
    printf("\t- D         Deletes pattern space until first newline. Behaves like d if no newline, else start a new cycle without reading input.\n");
    printf("\t- g         Copies hold space into pattern space.\n");
    printf("\t- G         Appends a newline, and the hold space to the pattern space.\n");
    printf("\t- h         Copies pattern space into hold space.\n");
    printf("\t- H         Appends a newline, and the pattern space to the hold space.\n");
    printf("\t- l         Outputs pattern space in a visually unambiguous form.\n");
    printf("\t- n         Prints pattern space and a newline, and reads a new line of input.\n");
    printf("\t- N         Reads a new line of input and appends it to pattern space, preceded by a newline.\n");
    printf("\t- p         Prints the pattern space and a newline.\n");
    printf("\t- P         Prints the pattern space until the first newline and a newline.\n");
    printf("\t- s/p/r/f   Searches in pattern space and replace matches of p by r. Flags: number, g, p, w\n");
    printf("\t- tlabel    Branches to label if a successful substitution was made.\n");
    printf("\t- wfile     Appends pattern space to file.\n");
    printf("\t- x         Exchanges pattern space and hold space.\n");
    printf("\t- y/a/b/    Replaces characters in a by corresponding characters in b.\n");
    printf("\n");
}

void init(command** ret, regex_module* regex) {
    memset(ret, 0, sizeof(*ret));
    define_command(ret, posix_lb, COMMAND, 0, 2, "{", 0);
    define_command(ret, posix_rb, COMMAND, 0, 0, "}", 0);
    define_command(ret, posix_a, COMMAND, 0, 1, "a%s\n", 0);
    define_command(ret, posix_b, COMMAND, 0, 2, "b%?s", ';');
    define_command(ret, posix_c, COMMAND, 0, 2, "c%s\n", 0);
    define_command(ret, posix_d, COMMAND, 0, 2, "d", 0);
    define_command(ret, posix_D, COMMAND, 0, 2, "D", 0);
    define_command(ret, posix_g, COMMAND, 0, 2, "g", 0);
    define_command(ret, posix_G, COMMAND, 0, 2, "G", 0);
    define_command(ret, posix_h, COMMAND, 0, 2, "h", 0);
    define_command(ret, posix_H, COMMAND, 0, 2, "H", 0);
    define_command(ret, posix_i, COMMAND, 0, 1, "i%s\n", 0);
    define_command(ret, posix_l, COMMAND, 0, 2, "l", 0);
    define_command(ret, posix_n, COMMAND, 0, 2, "n", 0);
    define_command(ret, posix_N, COMMAND, 0, 2, "N", 0);
    define_command(ret, posix_p, COMMAND, 0, 2, "p", 0);
    define_command(ret, posix_P, COMMAND, 0, 2, "P", 0);
    define_command(ret, posix_q, COMMAND, 0, 1, "q", 0);
    define_command(ret, posix_r, COMMAND, 0, 1, "r%s", ';');
    define_command(ret, posix_s, COMMAND, 0, 2, "s%1%s%1%s%1%s%?(w%s)", ';');
    define_command(ret, posix_t, COMMAND, 0, 2, "t%?s", ';');
    define_command(ret, posix_w, COMMAND, 0, 2, "w%s", ';');
    define_command(ret, posix_x, COMMAND, 0, 2, "x", 0);
    define_command(ret, posix_y, COMMAND, 0, 2, "y%1%s%1%s%1", 0);
    define_command(ret, posix_col, COMMAND, 0, 0, ":%s", ';');
    define_command(ret, posix_eq, COMMAND, 0, 1, "=", 0);
    define_command(ret, posix_com, COMMAND, 0, 0, "#%s\n", 0);
    define_command_pattern(ret, posix_line, ADDRESS, subtype_LINE, 1, "%l", 0);
    define_command(ret, posix_pattern1, ADDRESS, subtype_PATTERN, 1, "/%?s/", 0);
    define_command(ret, posix_pattern2, ADDRESS, subtype_PATTERN, 1, "\\%1%?s%1", 0);
    define_command(ret, posix_comma, DELIMITER, 0, 1, ",", 0);
    for (int i = 0; i < 128; i++)
        dispatch_array[i] = &null_func;
    dispatch_array['{'] = &posix_process_lb;
    dispatch_array['}'] = &posix_process_rb;
    dispatch_array['a'] = &posix_process_a;
    dispatch_array['b'] = &posix_process_b;
    dispatch_array['c'] = &posix_process_c;
    dispatch_array['d'] = &posix_process_d;
    dispatch_array['D'] = &posix_process_D;
    dispatch_array['g'] = &posix_process_g;
    dispatch_array['G'] = &posix_process_G;
    dispatch_array['h'] = &posix_process_h;
    dispatch_array['H'] = &posix_process_H;
    dispatch_array['i'] = &posix_process_i;
    dispatch_array['l'] = &posix_process_l;
    dispatch_array['n'] = &posix_process_n;
    dispatch_array['N'] = &posix_process_N;
    dispatch_array['p'] = &posix_process_p;
    dispatch_array['P'] = &posix_process_P;
    dispatch_array['q'] = &posix_process_q;
    dispatch_array['r'] = &posix_process_r;
    dispatch_array['s'] = &posix_process_s;
    dispatch_array['t'] = &posix_process_t;
    dispatch_array['w'] = &posix_process_w;
    dispatch_array['x'] = &posix_process_x;
    dispatch_array['y'] = &posix_process_y;
    dispatch_array['='] = &posix_process_eq;
}

void* prepare(token* tok) {
    label *labels = (label*) malloc(MAXLABELS*sizeof(label));
    int idx = 0;
    memset(labels, 0, MAXLABELS*sizeof(label));
    for (;tok;tok=tok->nexttoken)
        if (tok->command == ':') {
            assert(tok->args, ": expects an argument\n", NULL);
            labels[idx].key = tok->args[0];
            labels[idx].target = tok;
            idx++;
        }
    return labels;
}

bool match_address(int line_number, char* line, char* addr, int subtype) {
    if (subtype == subtype_LINE)
        return (line_number == atoi(addr));
    else
        return true; //TODO: regex
}

bool match(const int line_number, const char* line, token* tok) {
    if (flag_ignore)
        return false;
    if (!(tok->addresses))
        return true;
    if (tok->delimiter && tok->delimiter != ',')
        return false; //Should not happen
    if (!tok->delimiter)
        return match_address(line_number, line, tok->addresses[0], tok->subtypes[0]);
    else {
        bool ret;
        if (!(tok->in_range)) {
            ret = match_address(line_number, line, tok->addresses[0], tok->subtypes[0]);
            tok->in_range = ret;
        }
        else {
            ret = match_address(line_number, line, tok->addresses[1], tok->subtypes[1]);
            tok->in_range = !ret;
        }
        return ret;
    }
}

void process(unsigned char command, bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out) {
    dispatch_array[command](matched, tok, line_number, bufs, fl, labels, in, out);
}

void rm(command** commands) {
    bool pattern_removed = false;
    for (int i = 0; i < 128; i++)
        if ((i >= '0' && i <= '9') || i == '$') {
            if (!pattern_removed) {
                pattern_removed = true;
                free(commands[i]);
            }
        }
        else
            free(commands[i]);
}
