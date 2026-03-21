#pragma once

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
} Token;

// it strdup's lexeme
static inline Token new_tok(TokenType t, char *lexeme, size_t idx) {
  return (Token){t, strdup(lexeme), idx};
}

typedef struct {
  Token *items;
  size_t count;
  size_t capacity;
} Tokens;

static inline void free_token(Token *tok) { free(tok->lexeme); }

static inline void free_tokens(Tokens *toks) {
  for (size_t i = 0; i < toks->count; i++) {
    free(toks->items[i].lexeme);
  }
  free(toks->items);
}
