#pragma once

#include <stddef.h>

typedef enum {
  COMMAND,  // e.g., ls, echo, git
  ARGUMENT, // e.g., file names, options

  REDIRECT_IN,     // <
  REDIRECT_OUT,    // >
  REDIRECT_APPEND, // >>

  PIPE, // |

  AND, // &&
  OR,  // ||

  BACKGROUND, // &

  LPAREN, // (
  RPAREN, // )

  STRING, // "hello world" or 'hello world'

  VARIABLE, // $HOME, $PATH, etc.

  END_OF_FILE,
  UNKNOWN
} TokenType;

typedef struct {
  TokenType type;
  char *lexeme;
  size_t idx;
} Token;

Token *tok_new(TokenType t, const char *lexeme, size_t idx);
