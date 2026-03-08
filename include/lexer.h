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

char peek(Lexer lexer);

bool is_eof(Lexer *lexer);

void skip_whitespace(Lexer *lexer);

Token *read_identifier(Lexer *lexer);

Token *read_number(Lexer *lexer);

Token *read_symbol(Lexer *lexer);

Token *next_token(Lexer *lexer);

TokenArr Lex(const char *src);
