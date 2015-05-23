#include "compile.h"
#include "module.h"
#include "regex.h"
#include "util.h"

typedef struct script {
    char* text;
    token* tokens;
    void* mod_data;
    struct script *next;
    bool is_file;
    bool compiled;
} script;

int sed (module* mod, script* scripts, FILE* in, FILE* out, bool quiet);
