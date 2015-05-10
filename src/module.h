#include "util.h"
#include <dlfcn.h>

int init_mod(module **mod, char* modfile, bool printmode);
void rm_mod(module *mod);
