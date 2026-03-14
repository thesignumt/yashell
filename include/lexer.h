#pragma once

#include "token.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  const char *input;
  size_t idx;
  char current;
} Lexer;

Lexer init_lexer(const char *input);

void advance(Lexer *lexer);
void advancen(Lexer *lexer, size_t n);

char peek(Lexer lexer);
char peekn(Lexer lexer, size_t n);

bool is_eof(Lexer *lexer);

void skip_whitespace(Lexer *lexer);

Token *read_identifier(Lexer *lexer);

Token *read_string(Lexer *lexer);

Token *read_symbol(Lexer *lexer);

Token *next_token(Lexer *lexer);

TokenArr Lex(const char *src);
