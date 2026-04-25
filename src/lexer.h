#ifndef LEXER_H_
#define LEXER_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    STDIN_REDIRECT,  // <
    STDOUT_REDIRECT, // >
    REDIRECT_APPEND, // >>
    PIPE,            // |
    BACKGROUND,      // &
    IDENTIFIER,      // commands & arguments
    STRING,          // "quoted args"
    END_OF_FILE,     // end of input
    UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char *lexeme;
    size_t idx;
    size_t len;
} Token;

static inline Token new_tok(TokenType t, char *lexeme, size_t idx, size_t len) {
    return (Token){t, strdup(lexeme), idx, len};
}

typedef struct { // TODO: use zero memory
    Token *items;
    size_t count;
    size_t capacity;
} Tokens;

static inline void free_tokens(Tokens *toks) {
    for (size_t i = 0; i < toks->count; i++)
        free(toks->items[i].lexeme);
    free(toks->items);
}

typedef struct {
    const char *input;
    size_t idx;
    size_t len;
    char current;
} LexState;

Tokens Lex(const char *src);

#endif // LEXER_H_
