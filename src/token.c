#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

/*
 * NOTE: make sure to free(tok->lexeme) and free(tok) when done
 */
Token* new_tok(TokenType t, const char* lexeme, size_t idx) {
  Token* tok = malloc(sizeof(Token));
  if (!tok) return NULL;

  tok->type = t;
  tok->lexeme = strdup(lexeme);
  tok->idx = idx;
  return tok;
}
