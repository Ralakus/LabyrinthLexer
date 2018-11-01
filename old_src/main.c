#define LAB_LEXER_MAX_RULE_COUNT 6

#include "logger.h"
#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <time.h>

typedef enum tokens_e {
    tok_nil,

    tok_whitespace_space,
    tok_whitespace_tab,
    tok_whitespace_return,
    tok_whitespace_newline,

    tok_identifier,
    tok_number,

    tok_char,
    tok_string,

    tok_lparen,
    tok_rparen,
    tok_lbracket,
    tok_rbracket,
    tok_lcurley,
    tok_rcurley,

    tok_comma,
    tok_colon,
    tok_semicolon,

    tok_func,
    tok_let,
    tok_return,

    tok_operator_plus,
    tok_operator_minus,
    tok_operator_mul,
    tok_operator_div,
    tok_operator_equals,
    tok_operator_xor,
    tok_operator_and,
    tok_operator_lesst,
    tok_operator_greatert,
    tok_operator_or,
    tok_operator_not,

    tok_eof,

}   tokens_e;

#define TOK_TO_STRING_TEMPLATE(token, string) case token: {                                                                 \
                                                  buffer = (char*)malloc(strlen(string)+1);                                 \
                                                  if(buffer==NULL) {                                                        \
                                                      lab_errorln("Failed to allocate buffer for function "#string"!");     \
                                                      return NULL;                                                          \
                                                  }                                                                         \
                                                  buffer[strlen(string)] = '\0';                                            \
                                                  strcpy(buffer, string);                                                   \
                                                  break;                                                                    \
                                              }

char* tok_to_string(tokens_e tok) {
    char* buffer = NULL;
    switch(tok) {
        TOK_TO_STRING_TEMPLATE(tok_nil, "nil")
        TOK_TO_STRING_TEMPLATE(tok_whitespace_space, "whitespace space")
        TOK_TO_STRING_TEMPLATE(tok_whitespace_tab, "whitespace tab")
        TOK_TO_STRING_TEMPLATE(tok_whitespace_newline, "whitespace newline")
        TOK_TO_STRING_TEMPLATE(tok_whitespace_return, "whitespace return")
        TOK_TO_STRING_TEMPLATE(tok_identifier, "identifier")
        TOK_TO_STRING_TEMPLATE(tok_number, "number")
        TOK_TO_STRING_TEMPLATE(tok_char, "char")
        TOK_TO_STRING_TEMPLATE(tok_string, "string")
        TOK_TO_STRING_TEMPLATE(tok_lparen, "left paren")
        TOK_TO_STRING_TEMPLATE(tok_rparen, "right paren")
        TOK_TO_STRING_TEMPLATE(tok_lbracket, "left bracket")
        TOK_TO_STRING_TEMPLATE(tok_rbracket, "right bracket")
        TOK_TO_STRING_TEMPLATE(tok_lcurley, "left curley")
        TOK_TO_STRING_TEMPLATE(tok_rcurley, "right curley")
        TOK_TO_STRING_TEMPLATE(tok_comma, "comma")
        TOK_TO_STRING_TEMPLATE(tok_colon, "colon")
        TOK_TO_STRING_TEMPLATE(tok_semicolon, "semicolon")
        TOK_TO_STRING_TEMPLATE(tok_func, "function")
        TOK_TO_STRING_TEMPLATE(tok_let, "let")
        TOK_TO_STRING_TEMPLATE(tok_return, "return")
        TOK_TO_STRING_TEMPLATE(tok_operator_plus, "operator plus")
        TOK_TO_STRING_TEMPLATE(tok_operator_minus, "operator minus")
        TOK_TO_STRING_TEMPLATE(tok_operator_mul, "operator multiply")
        TOK_TO_STRING_TEMPLATE(tok_operator_div, "operator divide")
        TOK_TO_STRING_TEMPLATE(tok_operator_equals, "operator equals")
        TOK_TO_STRING_TEMPLATE(tok_operator_xor, "operator xor")
        TOK_TO_STRING_TEMPLATE(tok_operator_and, "operator and")
        TOK_TO_STRING_TEMPLATE(tok_operator_lesst, "operator less than")
        TOK_TO_STRING_TEMPLATE(tok_operator_greatert, "operator greater than")
        TOK_TO_STRING_TEMPLATE(tok_operator_or, "operator or")
        TOK_TO_STRING_TEMPLATE(tok_operator_not, "operator not")
        default: {
            break;
        }
    }
    return buffer;
}

lab_lexer_token_t alpha_callback(const char* code, size_t* iter, size_t max_len, void* user_data) {
    size_t begin_iter = *iter;

    static const char* reserved[]    = {    "Func",   "let",   "return" };
    static tokens_e reserved_types[] = { tok_func, tok_let, tok_return };

    for(;*iter < max_len; (*iter)++ ) {

        if(!isalpha(code[(*iter) + 1]) && !isdigit(code[(*iter) + 1])) {

            for(size_t i = 0; i < (sizeof(reserved) / sizeof(const char*)); i++) {

                for(size_t j = 0;; j++) {

                    if(j >= (*iter) - begin_iter) {

                        return lab_lexer_token_make((int)reserved_types[i], NULL);

                    } else if(reserved[i][j]=='\0') {
                        break;
                    }
                    if(reserved[i][j]==(code + begin_iter)[j]) {
                        continue;
                    } else {
                        break;
                    }
                }
            }

            char* ident = (char*)malloc(((*iter) - begin_iter) + 2);
            if(ident==NULL) {

                lab_lexer_position_t begin_pos = lab_lexer_position_from_iter(code, begin_iter);
                lab_errorln("Failed to allocate buffer for identifier token for identifier at line: %d, column: %d!", begin_pos.line, begin_pos.column);

            } else {

                ident[((*iter) - begin_iter) + 1] = '\0';
                memcpy(ident, code + begin_iter, ((*iter) - begin_iter) + 1);

            }

            return lab_lexer_token_make((int)tok_identifier, ident);
        }
    }
    return lab_lexer_token_make((int)tok_nil, NULL);
}

lab_lexer_token_t whitespace_callback(const char* code, size_t* iter, size_t max_len, void* user_data) {
    char type = code[(*iter)];
    char* type_name = NULL;
    switch (type) {
        case ' ' : return lab_lexer_token_make((int)tok_whitespace_space, NULL);
        case '\t': return lab_lexer_token_make((int)tok_whitespace_tab, NULL);
        case '\n': return lab_lexer_token_make((int)tok_whitespace_newline, NULL);
        case '\r': return lab_lexer_token_make((int)tok_whitespace_return, NULL);
        default  : return lab_lexer_token_make((int)tok_nil, type_name);;
    }
}

lab_lexer_token_t numeric_callback(const char* code, size_t* iter, size_t max_len, void* user_data) {
    size_t begin_iter = *iter;

    for(;*iter < max_len; (*iter)++ ) {

        if(!isdigit(code[(*iter) + 1]) && code[(*iter) + 1] != '.') {

            char* num = (char*)malloc(((*iter) - begin_iter) + 2);
            if(num==NULL) {

                lab_lexer_position_t pos = lab_lexer_position_from_iter(code, *iter);
                lab_errorln("Failed to allocate buffer for numerical token for number at line: %d, column: %d!", pos.line, pos.column);

            } else {

                num[((*iter) - begin_iter) + 1] = '\0';
                memcpy(num, code + begin_iter, ((*iter) - begin_iter) + 1);

            }

            return lab_lexer_token_make((int)tok_number, num);
        }
    }
    return lab_lexer_token_make((int)tok_nil, NULL);
}

lab_lexer_token_t symbol_callback(const char* code, size_t* iter, size_t max_len, void* user_data) {
    switch(code[(*iter)]) {
        case '(': return lab_lexer_token_make((int)tok_lparen,    NULL);
        case ')': return lab_lexer_token_make((int)tok_rparen,    NULL);
        case '[': return lab_lexer_token_make((int)tok_lbracket,  NULL);
        case ']': return lab_lexer_token_make((int)tok_rbracket,  NULL);
        case '{': return lab_lexer_token_make((int)tok_lcurley,   NULL);
        case '}': return lab_lexer_token_make((int)tok_rcurley,   NULL);
        case ',': return lab_lexer_token_make((int)tok_comma,     NULL);
        case ':': return lab_lexer_token_make((int)tok_colon,     NULL);
        case ';': return lab_lexer_token_make((int)tok_semicolon, NULL);
        default:  return lab_lexer_token_make((int)tok_nil,       NULL);
    }
}

lab_lexer_token_t operator_callback(const char* code, size_t* iter, size_t max_len, void* user_data) {
    switch(code[(*iter)]) {
        case '+': return lab_lexer_token_make((int)tok_operator_plus,         NULL);
        case '-': return lab_lexer_token_make((int)tok_operator_minus,        NULL);
        case '*': return lab_lexer_token_make((int)tok_operator_mul,          NULL);
        case '/': return lab_lexer_token_make((int)tok_operator_div,          NULL);
        case '=': return lab_lexer_token_make((int)tok_operator_equals,       NULL);
        case '^': return lab_lexer_token_make((int)tok_operator_xor,          NULL);
        case '<': return lab_lexer_token_make((int)tok_operator_lesst,        NULL);
        case '>': return lab_lexer_token_make((int)tok_operator_greatert,     NULL);
        case '|': return lab_lexer_token_make((int)tok_operator_or,           NULL);
        case '!': return lab_lexer_token_make((int)tok_operator_not,          NULL);
        default:  return lab_lexer_token_make((int)tok_nil,                   NULL);
    }
}

lab_lexer_token_t string_callback(const char* code, size_t* iter, size_t max_len, void* user_data) {
    int mode = code[(*iter)]=='\"' ? 1 : -1; // 1 means it's lexing a string, -1 means char
    size_t begin_index = (*iter);
    size_t end_index = 0;
    ++(*iter);
    for(;*iter < max_len; (*iter)++ ) {
        if(code[(*iter)]=='\"' && mode == 1) {
            end_index = (*iter) - 1;
            break;
        } else if(code[(*iter)]=='\'' && mode == -1) {
            end_index = (*iter) -1;
            break;
        }
    }
    if(end_index==0) {
        lab_lexer_position_t begin_pos = lab_lexer_position_from_iter(code, begin_index);
        lab_errorln("Failed to find string closing statement for string starting at line: %d, column: %d", begin_pos.line, begin_pos.column);
        return lab_lexer_token_make((int)tok_nil, NULL);
    } else {
        char* buffer = (char*)malloc((end_index - begin_index) + 1);
        if(buffer==NULL) {
            lab_lexer_position_t begin_pos = lab_lexer_position_from_iter(code, begin_index);
            lab_errorln("Failed to allocate string buffer for string starting at line: %d, column: %d", begin_pos.line, begin_pos.column);
            return lab_lexer_token_make((int)tok_nil, NULL);
        }
        buffer[end_index - begin_index] = '\0';
        memcpy(buffer, code + begin_index + 1, end_index - begin_index);
        return lab_lexer_token_make(mode == 1 ? (int)tok_string : (int)tok_char, buffer);
    }
    return lab_lexer_token_make((int)tok_nil, NULL);
}

/*
    TODO: Find a way to make the lexer take an a null termination as a rule
*/
lab_lexer_token_t eof_callback(const char* code, size_t* iter, size_t max_len, void* user_data) {
    return lab_lexer_token_make((int)tok_eof, NULL);
}

/*
    TODO: Make argument parser
*/
int main(int argc, char* argv[]) {

    clock_t start, end;

    size_t file_count           = argc - 1;
    char** file_names           = NULL;
    size_t* file_name_sizes     = NULL;
    char** file_contents        = NULL;
    size_t* file_contents_sizes = NULL;

    if(argc > 1) {
        FILE* cur_file = NULL;
        file_names          = (char**)malloc(sizeof(char*)  * file_count);
        file_name_sizes     = (size_t*)malloc(sizeof(size_t) * file_count);
        file_contents       = (char**)malloc(sizeof(char*)  * file_count);
        file_contents_sizes = (size_t*)malloc(sizeof(size_t) * file_count);

        if(file_names==NULL) {
            lab_errorln("Failed to allocate file name buffer!");
            free(file_names);
            free(file_name_sizes);
            free(file_contents);
            free(file_contents_sizes);
            return 1;
        } else if(file_name_sizes==NULL) {
            lab_errorln("Failed to allocate file name size buffer!");
            free(file_names);
            free(file_name_sizes);
            free(file_contents);
            free(file_contents_sizes);
            return 1;
        } else if(file_contents==NULL) {
            lab_errorln("Failed to allocate file contents buffer!");
            free(file_names);
            free(file_name_sizes);
            free(file_contents);
            free(file_contents_sizes);
            return 1;
        } else if(file_contents_sizes==NULL) {
            lab_errorln("Failed to allocate file contents size buffer!");
            free(file_names);
            free(file_name_sizes);
            free(file_contents);
            free(file_contents_sizes);
            return 1;
        }

        for(int i = 1; i < argc; i++) {

            file_name_sizes[i-1] = strlen(argv[i])+1;
            file_names[i-1] = (char*)malloc(file_name_sizes[i-1]);
            if(file_names[i-1]==NULL) {
                lab_errorln("Failed to allocate file name for file: \"%s\"!", argv[i]);
                for(int j = 0; j < file_count; j++) {
                    free(file_names[j]);
                    free(file_contents[j]);
                }
                free(file_names);
                free(file_name_sizes);
                free(file_contents);
                free(file_contents_sizes);
                return 1;
            }

            file_names[i-1][file_name_sizes[i-1]-1] = '\0';
            memcpy(file_names[i-1], argv[i], file_name_sizes[i-1]-1);

            cur_file = fopen(argv[i], "r");

            if(cur_file==NULL) {
                lab_errorln("Failed to openfile: \"%s\"!", argv[i]);
                for(int j = 0; j < file_count; j++) {
                    free(file_names[j]);
                    free(file_contents[j]);
                }
                free(file_names);
                free(file_name_sizes);
                free(file_contents);
                free(file_contents_sizes);
                return 1;
            }

            fseek(cur_file, 0, SEEK_END);
            file_contents_sizes[i-1] = ftell(cur_file)+1;
            fseek(cur_file, 0, SEEK_SET);
            file_contents[i-1] = (char*)malloc(file_contents_sizes[i-1]);

            if(file_contents[i-1]==NULL) {
                lab_errorln("Failed to allocate file buffer for file: \"%s\"!", argv[i]);
                for(int j = 0; j < file_count; j++) {
                    free(file_names[j]);
                    free(file_contents[j]);
                }
                free(file_names);
                free(file_name_sizes);
                free(file_contents);
                free(file_contents_sizes);
                return 1;
            }

            file_contents[i-1][file_contents_sizes[i-1]-1] = '\0';

            fread(file_contents[i-1], 1, file_contents_sizes[i-1], cur_file);
            fclose(cur_file);
        }
    }
    else {
        lab_errorln("No input files!");
        return 1;
    }

    lab_lexer_ruleset_t rules;
    lab_lexer_ruleset_init(&rules);

    lab_lexer_token_container_t tokens;
    
    lab_lexer_add_rule(&rules, "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm", alpha_callback);
    lab_lexer_add_rule(&rules, " \n\t\r", whitespace_callback);
    lab_lexer_add_rule(&rules, "1234567890", numeric_callback);
    lab_lexer_add_rule(&rules, "()[]{},:;", symbol_callback);
    lab_lexer_add_rule(&rules, "+-*/=^&<>|", operator_callback);
    lab_lexer_add_rule(&rules, "\"\'", string_callback);

    start = clock();
    for(size_t i = 0; i < file_count; i++) {
        lab_lexer_token_container_init(&tokens);
        lab_lexer_lex(&tokens, file_contents[i], file_contents_sizes[i]-1, &rules, NULL);

        lab_noticeln("Tokens for file: \"%s\"", file_names[i]);
        /*for(size_t j = 0; j < tokens.count; j++) {
            char* tok_str = tok_to_string((tokens_e)tokens.tokens[j].id);
            lab_println("Token: %s: %s", tok_str, tokens.tokens[j].data);
            free(tok_str);
        }*/
        lab_noticeln("END");

        lab_lexer_token_container_free(&tokens);
    }
    end = clock();

    lab_successln("Time to lex: %fms", (((double)(end - start)) / CLOCKS_PER_SEC) * 1000);

    return 0;
}