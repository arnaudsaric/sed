#include "regex.h"

int init_regex(regex_module **mod, char* regexfile, bool printmode) {
    void* handle = dlopen(regexfile, RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return -1;
    }
    if (printmode) {
        void* print = dlsym(handle, "print");
        ((print_func)print)(true);
        exit(EXIT_SUCCESS);
    }
    void* compile = dlsym(handle, "compile");
    if (!compile) {
        fprintf(stderr, "%s\n", dlerror());
        return -1;
    }
    void* match = dlsym(handle, "match");
    if (!match) {
        fprintf(stderr, "%s\n", dlerror());
        return -1;
    }
    void* exec = dlsym(handle, "exec");
    if (!exec) {
        fprintf(stderr, "%s\n", dlerror());
        return -1;
    }
    *mod = (regex_module*) malloc (sizeof(regex_module));
    assert(mod, "Could not allocate memory\n", -1);
    (*mod)->handle = handle;
    (*mod)->regex = NULL;
    (*mod)->compile = (regex_compile_func)compile;
    (*mod)->match = (regex_match_func)match;
    (*mod)->exec = (regex_exec_func)exec;
    return 0;
}

void rm_regex(regex_module *mod) {
    void* rm = dlsym(mod->handle, "rm");
    if (!rm) {
        fprintf(stderr, "%s\n", dlerror());
        return;
    }
    ((regex_rm_func)rm)(mod->regex);
    dlclose(mod->handle);
    free(mod);
}
