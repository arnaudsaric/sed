#ifndef UTIL
#define UTIL

#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char bool;

#define BUFSIZE 8096
#define true 1
#define false 0

#define assert(condition,msg,ret) \
    if (!condition) { \
        fprintf(stderr, msg); \
        return ret; \
    }

#define get_new_line(buf, file) \
    if (!fgets(buf, BUFSIZE, file)) \
        *buf = 0; \
    else \
        buf[strlen(buf) - 1] = 0

typedef struct token {
    char** addresses;
    int* subtypes;
    int n_addresses;
    char delimiter;
    bool address_negate;
    char command;
    char** args;
    int n_args;
    struct token *nexttoken;
    bool in_range;
} token;

typedef struct {
    char pattern[BUFSIZE];
    char hold[BUFSIZE];
    char lookahead[BUFSIZE];
    char append[BUFSIZE];
} buffers;

typedef struct {
    bool nextcycle;
    bool noprint;
    bool noread;
    bool stop;
} flags;

typedef struct command {
    enum {ADDRESS, COMMAND, DELIMITER} type;
    int subtype;
    int allow_address;
    char description[64];
    char nextchar;
} command;

typedef void (*init_func)(command**);
typedef void* (*prepare_func)(token*);
typedef bool (*match_func)(int,char*,bool*,char**,int*,char);
typedef void (*process_func)(bool,token**,int*,buffers*,flags*,void*,FILE*,FILE*);
typedef void (*dispatch_func)(char,bool,token**,int*,buffers*,flags*,void*,FILE*,FILE*);

typedef struct {
    void* handle;
    const command *commands[128];
    prepare_func prepare;
    match_func match;
    dispatch_func process;
} module;

#endif
