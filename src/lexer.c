#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "token.h"

Lexer init_lexer(const char *input) {
    Lexer lexer;
    lexer.input = input;
    lexer.idx = 0;
    lexer.len = strlen(input);
    lexer.current = input[0];
    return lexer;
}

void advance(Lexer *lexer) {
    if (lexer->idx < lexer->len)
        lexer->idx++;
    lexer->current =
        (lexer->idx < lexer->len) ? lexer->input[lexer->idx] : '\0';
}

// e = expected
int eat(Lexer *lexer, char e) {
    if (lexer->current == e) {
        advance(lexer);
        return 1;
    }
    return 0;
}

void skip_whitespace(Lexer *lexer) {
    while (isspace(lexer->current))
        advance(lexer);
}

// IDENTIFIER: command names, args, paths
Token read_identifier(Lexer *lexer) {
    size_t start = lexer->idx;
    size_t cap = 32;
    size_t len = 0;
    char *buffer = malloc(cap);

    // allow Windows drive prefix like C:
    if (isalpha(lexer->current)) {
        buffer[len++] = lexer->current;
        advance(lexer);

        if (lexer->current == ':') {
            buffer[len++] = lexer->current;
            advance(lexer);
        }
    }

    while (lexer->current && is_ident_char(lexer->current)) {
        if (len + 1 >= cap) {
            cap *= 2;
            buffer = realloc(buffer, cap);
            if (!buffer)
                exit(1);
        }
        buffer[len++] = lexer->current;
        advance(lexer);
    }

    buffer[len] = '\0';
    Token tok = new_tok(IDENTIFIER, buffer, start, len);
    free(buffer);
    return tok;
}

// STRING: quoted arguments
Token read_string(Lexer *lexer) {
    size_t start = lexer->idx;
    char quote = lexer->current;
    advance(lexer);

    char buffer[1024];
    size_t i = 0;

    while (lexer->current != quote && !is_eof_char(lexer->current) &&
           i < sizeof(buffer) - 1) {
        if (eat(lexer, '\\')) {
            if (lexer->current == quote || lexer->current == '\\') {
                buffer[i] = lexer->current;
                advance(lexer);
            } else
                buffer[i] = '\\';

        } else {
            buffer[i] = lexer->current;
            advance(lexer);
        }
        i++;
    }

    eat(lexer, quote);
    buffer[i] = '\0';

    return new_tok(STRING, buffer, start, lexer->idx - start);
}

// Symbols: < > >> | & :
Token read_symbol(Lexer *lexer) {
    size_t start = lexer->idx;
    char cur = lexer->current;

    // ignore ':' as a symbol (let identifiers consume it only if already inside
    // token)
    if (cur == ':') {
        advance(lexer);
        return new_tok(UNKNOWN, ":", start, 1);
    }

    if (eat(lexer, '<'))
        return new_tok(STDIN_REDIRECT, "<", start, lexer->idx - start);
    if (eat(lexer, '>')) {
        if (eat(lexer, '>'))
            return new_tok(REDIRECT_APPEND, ">>", start, lexer->idx - start);
        return new_tok(STDOUT_REDIRECT, ">", start, lexer->idx - start);
    }
    if (eat(lexer, '|'))
        return new_tok(PIPE, "|", start, lexer->idx - start);
    if (eat(lexer, '&'))
        return new_tok(BACKGROUND, "&", start, lexer->idx - start);

    advance(lexer);
    return new_tok(UNKNOWN, (char[]){cur, '\0'}, start, 1);
}

Token next_token(Lexer *lexer) {
    skip_whitespace(lexer);

    if (is_eof_char(lexer->current))
        return new_tok(END_OF_FILE, "", lexer->idx, 0);

    if (lexer->current == '\'' || lexer->current == '"')
        return read_string(lexer);

    if (is_ident_char(lexer->current))
        return read_identifier(lexer);

    return read_symbol(lexer);
}

// NOTE: EOF is a sentinel token used only by the parser.
Tokens Lex(const char *src) {
    Lexer lexer = init_lexer(src);
    Tokens toks = {0};
    toks.capacity = 16;
    toks.items = malloc(toks.capacity * sizeof(*toks.items));

    while (1) {
        Token token = next_token(&lexer);
        if (toks.count >= toks.capacity) {
            toks.capacity *= 2;
            toks.items =
                realloc(toks.items, toks.capacity * sizeof(*toks.items));
        }

        toks.items[toks.count++] = token;

        if (token.type == END_OF_FILE)
            break;
    }

    return toks;
}
