#include "module.h"

int init_mod(module **mod, char* modfile, bool printmode) {
    void* handle = dlopen(modfile, RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return -1;
    }
    if (printmode) {
        void* print = dlsym(handle, "print");
        ((print_func)print)(true);
        exit(EXIT_SUCCESS);
    }
    void* init = dlsym(handle, "init");
    void* prepare = dlsym(handle, "prepare");
    void* match = dlsym(handle, "match");
    void* process = dlsym(handle, "process");
    *mod = (module*) malloc (sizeof(module));
    (*mod)->handle = handle;
    ((init_func)init)((command**)(*mod)->commands);
    (*mod)->prepare = (prepare_func)prepare;
    (*mod)->match = (match_func)match;
    (*mod)->process = (dispatch_func)process;
    return 0;
}

void rm_mod(module *mod) {
    void* rm = dlsym(mod->handle, "rm");
    ((init_func)rm)((command**)mod->commands);
    dlclose(mod->handle);
    free(mod);
}
