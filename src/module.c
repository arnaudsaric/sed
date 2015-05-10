#include "module.h"

int init_mod(module **mod, char* modfile) {
    *mod = (module*) malloc (sizeof(module));
    void* handle = dlopen(modfile, RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return -1;
    }
    void* init = dlsym(handle, "init");
    void* prepare = dlsym(handle, "prepare");
    void* match = dlsym(handle, "match");
    void* process = dlsym(handle, "process");
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
