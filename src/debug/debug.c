#include "compile.h"
#include "module.h"

int main (int argc, char** argv) {
    module* mod_posix;
    init_mod(&mod_posix, "modules/posix.so");
    token* first = compile(mod_posix->commands, argv[1]);
    token* tok = first;
    for (;tok;tok = tok->nexttoken) {
        int i = 0;
        printf("Token: ");
        for (;i<tok->n_addresses;i++)
            printf("addr%d=%s ",i,tok->addresses[i]);
        printf("delim=%c ",tok->delimiter);
        printf("command=%c ",tok->command);
        for (i=0;i<tok->n_args;i++)
            printf("arg%d=%s ",i,tok->args[i]);
        printf("\n");
    }
}
