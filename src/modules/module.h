#include "util.h"

#define define_command(_array, _name, _type, _subtype, _naddr, _pattern, _nextchar) \
    command* _name = (command*) malloc(sizeof(command)); \
    _name->type = _type; \
    _name->subtype = _subtype; \
    _name->allow_address = _naddr; \
    strcpy(_name->description,_pattern); \
    _name->nextchar = _nextchar; \
    _array[(unsigned char)_name->description[0]] = _name;

#define define_command_noassign(_name, _type, _subtype, _naddr, _pattern, _nextchar) \
    command* _name = (command*) malloc(sizeof(command)); \
    _name->type = _type; \
    _name->subtype = _subtype; \
    _name->allow_address = _naddr; \
    strcpy(_name->description,_pattern); \
    _name->nextchar = _nextchar;

#define define_command_pattern(_array, _name, _type, _subtype, _naddr, _pattern, _nextchar) \
    command* _name = (command*) malloc(sizeof(command)); \
    _name->type = _type; \
    _name->subtype = _subtype; \
    _name->allow_address = _naddr; \
    strcpy(_name->description,_pattern); \
    _name->nextchar = _nextchar; \
    _array['0'] = _name; \
    _array['1'] = _name; \
    _array['2'] = _name; \
    _array['3'] = _name; \
    _array['4'] = _name; \
    _array['5'] = _name; \
    _array['6'] = _name; \
    _array['7'] = _name; \
    _array['8'] = _name; \
    _array['9'] = _name; \
    if (_name->description[1] == 'l') \
        _array['$'] = _name;

void null_func(bool matched, token** tok, int* line_number, buffers* bufs, flags* fl, void* labels, FILE* in, FILE* out);
