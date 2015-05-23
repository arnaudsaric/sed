#include "module.h"
#include "regex.h"
#include "sed.h"
#include "util.h"
#include <argp.h>

typedef struct in {
    char* file;
    struct in *next;
} in;

typedef struct {
    bool quiet;
    script* scripts;
    in* ins;
    char* module;
    char* regex_mod;
} options;

error_t option_parser (int key, char* arg, struct argp_state* state) {
    options* input = (options*)(state->input);
    if (key == 'n') {
        input->quiet=true;
        return 0;
    }
    if (key == 'M') {
        input->module = (char*) malloc(strlen(arg));
        strcpy(input->module, (const char*) arg);
        return 0;
    }
    if (key == 'R') {
        input->regex_mod = (char*) malloc(strlen(arg));
        strcpy(input->regex_mod, (const char*) arg);
        return 0;
    }
    if (key == ARGP_KEY_ARG) {
        if (!input->scripts) {
            input->scripts = (script*) malloc(sizeof(script));
            input->scripts->is_file = false;
            input->scripts->text = (char*) malloc(strlen(arg));
            strcpy(input->scripts->text, (const char*) arg);
            input->scripts->next = 0;
            return 0;
        }
        in* i;
        if (!input->ins) {
            input->ins = (in*) malloc(sizeof(in));
            i = input->ins;
        }
        else {
            for (i = input->ins;i->next;i=i->next);
            i->next = (in*) malloc(sizeof(in));
            i = i->next;
        }
        i->file = (char*) malloc(strlen(arg));
        strcpy(i->file, (const char*) arg);
        i->next = 0;
        return 0;
    }
    if (key != 'e' && key != 'f')
        return ARGP_ERR_UNKNOWN;
    script* s;
    if (!input->scripts) {
        input->scripts = (script*) malloc(sizeof(script));
        s = input->scripts;
    }
    else {
        for (s=input->scripts;s->next;s = s->next);
        s->next = (script*) malloc(sizeof(script));
        s = s->next;
    }
    s->is_file = (key == 'f');
    s->text = (char*) malloc(strlen(arg));
    strcpy(s->text, (const char*) arg);
    s->next = 0;
    return 0;
}

int main (int argc, char ** argv) {
    const struct argp_option parser_options[7] = {
        {.name = "quiet",      .key = 'n', .arg = NULL,                .flags = 0,            .doc = "No automatic printing of pattern space"},
        {.name = "silent",     .key = 'n', .arg = NULL,                .flags = OPTION_ALIAS, .doc = NULL},
        {.name = "expression", .key = 'e', .arg = "script",            .flags = 0,            .doc = "A script string to be executed"},
        {.name = "file",       .key = 'f', .arg = "script-file",       .flags = 0,            .doc = "A file containing a script to be executed"},
        {.name = "module",     .key = 'M', .arg = "module-file",       .flags = 0,            .doc = "(mandatory) An msed module"},
        {.name = "regex",      .key = 'R', .arg = "regex-module-file", .flags = 0,            .doc = "(mandatory) A regex module"}
    };
    const struct argp_child child = {0};
    struct argp parser = {
        .options = parser_options,
        .parser = &option_parser,
        .args_doc = "\ninput-files\nscript [input-files]",
        .doc = "MSed v0.0.1, a Modular Stream EDitor\vIf no -e or -f option is specified, the first non-option argument will be used as a script and all the others as input files. Otherwise, all non-option arguments are used as input files. \033[1mTo get information on the commands of a module, type \"sed -M module-file\".\033[0m",
        .children = &child,
        .help_filter = NULL,
        .argp_domain = NULL,
    };
    module* mod;
    regex_module* regex_mod;
    options opts = {0};
    argp_parse(&parser, argc, argv, 0, NULL, &opts);
    if (!opts.module) {
        fprintf(stderr, "No module specified! Here is usage:\n");
        execl(argv[0], argv[0], "--help", NULL);
        exit(EXIT_FAILURE);
    }
    if (!opts.regex_mod) {
        fprintf(stderr, "No regex module specified! Here is usage:\n");
        execl(argv[0], argv[0], "--help", NULL);
        exit(EXIT_FAILURE);
    }
    init_regex(&regex_mod, opts.regex_mod, false);
    init_mod(&mod, opts.module, !opts.scripts, regex_mod);
    if (!opts.ins)
        exit(sed(mod, opts.scripts, stdin, stdout, opts.quiet));
    FILE *file = 0;
    int ret = 0;
    for (in* i = opts.ins; i; i = i->next)
        if (!(file = fopen(i->file, "r")))
            fprintf(stderr, "Could not open %s\n", i->file);
        else
            ret += sed(mod, opts.scripts, file, stdout, opts.quiet);
    rm_regex(regex_mod);
    rm_mod(mod);
    exit(ret);
}
