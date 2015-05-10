#ifndef COMPILE
#define COMPILE

#include "util.h"

#define shift() \
    in++; \
    assert(*in, "premature end of input\n", -1)

#define fill_step(buffer) \
    buffer[i] = *in; \
    i++; \
    assert((i < BUFSIZE - 1), "address too long\n", -1) \
    shift()

#define fill_step_canEOF(buffer) \
    buffer[i] = *in; \
    i++; \
    assert((i < BUFSIZE - 1), "address too long\n", -1) \
    in++;

#define fill_while(buffer, condition) \
    while (condition) { \
        fill_step(buffer) \
    }

#define fill_while_canEOF(buffer, condition) \
    while (condition) { \
        fill_step_canEOF(buffer) \
    }

token* compile(const command** valid_commands, char* commands);

#endif
