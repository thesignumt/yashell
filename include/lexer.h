#ifndef LEXER_H_
#define LEXER_H_

#include "token.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  const char *input;
  size_t idx;
  size_t len;
  char current;
} Lexer;

Lexer init_lexer(const char *input);

void advance(Lexer *lexer);
void advancen(Lexer *lexer, size_t n);

int eat(Lexer *lexer, char e);

char peek(Lexer lexer);
char peekn(Lexer lexer, size_t n);

static inline bool is_eof_char(char c) { return c == '\0'; }
static inline bool is_ident_char(char c) {
  return isalnum((unsigned char)c) || c == '.' || c == '/' || c == '-' ||
         c == '_';
}

void skip_whitespace(Lexer *lexer);

Token read_identifier(Lexer *lexer);

Token read_string(Lexer *lexer);

Token read_symbol(Lexer *lexer);

Token next_token(Lexer *lexer);

Tokens Lex(const char *src);

#endif  // LEXER_H_
