#include "helper_macros.h"

#include <stddef.h>
#include <string.h>

#ifndef LAB_ARG_PARSER_MAX_ARG_TYPES 

    #define LAB_ARG_PARSER_MAX_ARG_TYPES 16

#endif

#define LAB_ARG_PARSER_WILDCARD -1

typedef struct lab_arg_t {

    const char*  long_name;
    const char* short_name;
    int id                ;

} lab_arg_t;

typedef struct lab_arg_parser_rules_t {

    size_t      count;
    const char* docs;
    const char* version;
    lab_arg_t   valid_args[LAB_ARG_PARSER_MAX_ARG_TYPES];

} lab_arg_parser_rules_t;

int lab_get_next_arg(int argc, char** argv, size_t* arg_iter, lab_arg_parser_rules_t* rules) {
    ++(*arg_iter);
    for(size_t i = 0; i < LAB_ARG_PARSER_MAX_ARG_TYPES; i++) {
        if(strcmp(argv[*arg_iter] + 2, rules->valid_args[i].long_name)==0) { 

            return rules->valid_args[i].id;

        } else if(strcmp(argv[*arg_iter] + 1, rules->valid_args[i].short_name)==0) {

            return rules->valid_args[i].id;

        }
    }
    return LAB_ARG_PARSER_WILDCARD;
}