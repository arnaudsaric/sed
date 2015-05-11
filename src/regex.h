#include "util.h"
#include <dlfcn.h>

int init_regex(regex_module **mod, char* regexfile, bool printmode);
void rm_regex(regex_module *mod);
