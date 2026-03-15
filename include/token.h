#pragma once

#include <stddef.h>

typedef enum {
  STDIN_REDIRECT,  // <
  STDOUT_REDIRECT, // >
  REDIRECT_APPEND, // >>
  PIPE,            // |
  BACKGROUND,      // &
  IDENTIFIER,      // commands & arguments
  STRING,          // "quoted args"
  END_OF_FILE,     // end of input
} TokenType;

typedef struct {
  TokenType type;
  char *lexeme;
  size_t idx;
} Token;

Token *new_tok(TokenType t, const char *lexeme, size_t idx);

typedef struct {
  Token *tokens;
  size_t count;
} TokenArr;
} Tokens;
