#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static LexState init_lexer(const char *input) {
    LexState ls;
    ls.input = input;
    ls.idx = 0;
    ls.len = strlen(input);
    ls.current = input[0];
    return ls;
}

static int is_eof_char(char c) { return c == '\0'; }
static int is_ident_char(char c) {
    return isalnum((unsigned char)c) || c == '.' || c == '/' || c == '-' ||
           c == '_' || c == '~';
}

static void advance(LexState *ls) {
    if (ls->idx < ls->len)
        ls->idx++;
    ls->current = (ls->idx < ls->len) ? ls->input[ls->idx] : '\0';
}

// e = expected
static int eat(LexState *ls, char e) {
    if (ls->current == e) {
        advance(ls);
        return 1;
    }
    return 0;
}

static void skip_whitespace(LexState *lexer) {
    while (isspace(lexer->current))
        advance(lexer);
}

// IDENTIFIER: command names, args, paths
static Token read_identifier(LexState *ls) {
    size_t start = ls->idx;
    size_t cap = 32;
    size_t len = 0;
    char *buffer = malloc(cap);

    // allow Windows drive prefix like C:
    if (isalpha(ls->current)) {
        buffer[len++] = ls->current;
        advance(ls);

        if (ls->current == ':') {
            buffer[len++] = ls->current;
            advance(ls);
        }
    }

    while (ls->current && is_ident_char(ls->current)) {
        if (len + 1 >= cap) {
            cap *= 2;
            buffer = realloc(buffer, cap);
            if (!buffer)
                exit(1);
        }
        buffer[len++] = ls->current;
        advance(ls);
    }

    buffer[len] = '\0';
    Token tok = new_tok(IDENTIFIER, buffer, start, len);
    free(buffer);
    return tok;
}

// STRING: quoted arguments
static Token read_string(LexState *ls) {
    size_t start = ls->idx;
    char quote = ls->current;
    advance(ls);

    char buffer[1024];
    size_t i = 0;

    while (ls->current != quote && !is_eof_char(ls->current) &&
           i < sizeof(buffer) - 1) {
        if (eat(ls, '\\')) {
            if (ls->current == quote || ls->current == '\\') {
                buffer[i] = ls->current;
                advance(ls);
            } else
                buffer[i] = '\\';

        } else {
            buffer[i] = ls->current;
            advance(ls);
        }
        i++;
    }

    eat(ls, quote);
    buffer[i] = '\0';

    return new_tok(STRING, buffer, start, ls->idx - start);
}

// Symbols: < > >> | & :
static Token read_symbol(LexState *ls) {
    size_t start = ls->idx;
    char cur = ls->current;

    // ignore ':' as a symbol (let identifiers consume it only if already inside
    // token)
    if (cur == ':') {
        advance(ls);
        return new_tok(UNKNOWN, ":", start, 1);
    }

    if (eat(ls, '<'))
        return new_tok(STDIN_REDIRECT, "<", start, ls->idx - start);
    if (eat(ls, '>')) {
        if (eat(ls, '>'))
            return new_tok(REDIRECT_APPEND, ">>", start, ls->idx - start);
        return new_tok(STDOUT_REDIRECT, ">", start, ls->idx - start);
    }
    if (eat(ls, '|'))
        return new_tok(PIPE, "|", start, ls->idx - start);
    if (eat(ls, '&'))
        return new_tok(BACKGROUND, "&", start, ls->idx - start);

    advance(ls);
    return new_tok(UNKNOWN, (char[]){cur, '\0'}, start, 1);
}

static Token next_token(LexState *ls) {
    skip_whitespace(ls);

    if (is_eof_char(ls->current))
        return new_tok(END_OF_FILE, "", ls->idx, 0);

    if (ls->current == '\'' || ls->current == '"')
        return read_string(ls);

    if (is_ident_char(ls->current))
        return read_identifier(ls);

    return read_symbol(ls);
}

// NOTE: EOF is a sentinel token used only by the parser.
Tokens Lex(const char *src) {
    LexState ls = init_lexer(src);
    Tokens toks = {0};
    toks.capacity = 16;
    toks.items = malloc(toks.capacity * sizeof(*toks.items));

    while (1) {
        Token token = next_token(&ls);
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
