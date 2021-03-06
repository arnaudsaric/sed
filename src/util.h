#ifndef UTIL
#define UTIL

#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef unsigned char bool;

#define BIG_BUFSIZE 131072
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
    void* mod_data;
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

typedef int (*regex_compile_func)(void**,const char*,bool);
typedef bool (*regex_match_func)(const void*,const char*);
typedef bool (*regex_exec_func)(const void*,const char*,int,int*,int*);
typedef void (*regex_rm_func)(void*);

typedef struct {
    void* handle;
    regex_rm_func rm;
    regex_compile_func compile;
    regex_match_func match;
    regex_exec_func exec;
} regex_module;

typedef void (*print_func)(bool);
typedef void (*modinit_func)(command**,regex_module*);
typedef void (*modrm_func)(command**);
typedef void* (*prepare_func)(token*);
typedef bool (*match_func)(int,char*,token*);
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
