#include "util.h"
#include <dlfcn.h>

int init_mod(module **mod, char* modfile, bool printmode, regex_module* regex);
void rm_mod(module *mod);
