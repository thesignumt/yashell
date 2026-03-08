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

  IDENTIFIER,
  STRING, // "hello world" or 'hello world'
  NUMBER,
  SYMBOL,

  VARIABLE, // $HOME, $PATH, etc.

  END_OF_FILE,
  UNKNOWN
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
