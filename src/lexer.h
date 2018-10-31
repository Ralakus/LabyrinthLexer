#include "helper_macros.h"
#include "logger.h"

#include <stddef.h>

#include <stdlib.h>
#include <string.h>


#ifndef LAB_LEXER_MAX_RULE_COUNT

    #define LAB_LEXER_MAX_RULE_COUNT 16

#endif

typedef struct lab_lexer_token_t {

    int id;
    const char* data;

} lab_lexer_token_t;

typedef struct lab_lexer_token_container_t {
    
    size_t alloc_count;
    size_t count;
    lab_lexer_token_t* tokens;

} lab_lexer_token_container_t;

typedef struct lab_lexer_position_t {

    size_t line;
    size_t column;

} lab_lexer_position_t;

typedef lab_lexer_token_t(*lab_lexer_rule_callback_t)(const char* code, size_t* iter, const size_t max_length, void* user_data);

typedef struct lab_lexer_rule_t {

    const char* rule;
    lab_lexer_rule_callback_t callback;

} lab_lexer_rule_t;

typedef struct lab_lexer_ruleset_t {

    size_t count;
    lab_lexer_rule_t rules[LAB_LEXER_MAX_RULE_COUNT];

} lab_lexer_ruleset_t;

LAB_FORCEINLINE void lab_lexer_ruleset_init(lab_lexer_ruleset_t* ruleset) {
    if(ruleset!=NULL) ruleset->count = 0;
}

LAB_FORCEINLINE void lab_lexer_token_container_init(lab_lexer_token_container_t* container) {
    container->alloc_count = 0;
    container->count  = 0;
    container->tokens = NULL;
}

#define NOT_SO_FAST_CEIL(x) ((float)(long)((x)+1))

LAB_FORCEINLINE int lab_token_container_append(lab_lexer_token_container_t* container, lab_lexer_token_t token, const size_t* iter, size_t max_code_len) {

    ++container->count;
    if(container->count > container->alloc_count) {
        container->alloc_count = //container->count * ( max_code_len / pos->iter + (max_code_len % pos->iter != 0));
        (size_t)NOT_SO_FAST_CEIL(container->count *
        (((float)max_code_len / (float)*iter) > 1.0f ? ((float)max_code_len / (float)*iter) : 1.0f));
    }

    if(container->tokens == NULL) { 
        container->tokens = (lab_lexer_token_t*)malloc(sizeof(lab_lexer_rule_t) * container->alloc_count);
    } else {
        container->tokens = (lab_lexer_token_t*)realloc(container->tokens, sizeof(lab_lexer_rule_t) * container->alloc_count);
    }

    if(container->tokens == NULL) {
        lab_errorln("Failed to reallocate and add rule!");
        return 1;
    }

    memcpy(&container->tokens[container->count-1], &token, sizeof(lab_lexer_token_t));

    return 0;
}

LAB_FORCEINLINE lab_lexer_token_t lab_lexer_token_make(int id, char* data) {
    lab_lexer_token_t token;
    token.id = id;
    token.data = data;
    return token;
}

LAB_FORCEINLINE int lab_lexer_add_rule(lab_lexer_ruleset_t* rules, const char* rule, lab_lexer_rule_callback_t callback) {

    ++rules->count;

    if(rules->count > LAB_LEXER_MAX_RULE_COUNT) {
        lab_errorln("Rule count exceeded!");
        return 1;
    }

    rules->rules[rules->count-1].callback = callback;
    rules->rules[rules->count-1].rule     = rule;

    return 0;
}

LAB_FORCEINLINE lab_lexer_position_t lab_lexer_position_from_iter(const char* code, size_t iter) {
    lab_lexer_position_t pos;
    pos.line   = 1;
    pos.column = 0;
    for(size_t i = 0; i < iter; i++) {
        if(code[iter]=='\n') {

            ++pos.line;
              pos.column = 0;

        } else{

            ++pos.column;

        }
    }
    return pos;
}

LAB_FORCEINLINE int lab_lexer_lex(lab_lexer_token_container_t* tokens, const char* code, size_t code_len, lab_lexer_ruleset_t* rules, void* user_data) {

    if(code_len == 0) {
        code_len = strlen(code);
    }

    size_t iter;

    for (iter = 0; iter < code_len; iter++) {
        
        char cur_char = code[iter];
        int found_callback = 0;

        for(size_t j = 0; j < rules->count; j++) {

            for(size_t k = 0;; k++) {
                if(rules->rules[j].rule[k]=='\0') {
                    break;
                }

                if(cur_char==rules->rules[j].rule[k]) {
                    lab_token_container_append(tokens, rules->rules[j].callback(code, &iter, code_len, user_data), &iter, code_len);
                    found_callback = 1;
                    break;
                }

            }

            if(found_callback==1) {
                break;
            }

        }

        if(found_callback==0) {
            lab_lexer_position_t pos = lab_lexer_position_from_iter(code, iter);
            lab_errorln("Unexpected character: \"%c\" at line: %d, column: %d", cur_char, pos.line, pos.column);
        }
    }
return 0;

}