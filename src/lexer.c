#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "token.h"

Lexer init_lexer(const char* input) {
  Lexer lexer;
  lexer.input = input;
  lexer.idx = 0;
  lexer.len = strlen(input);
  lexer.current = input[0];
  return lexer;
}

void advance(Lexer* lexer) {
  if (lexer->idx < lexer->len) lexer->idx++;
  lexer->current = (lexer->idx < lexer->len) ? lexer->input[lexer->idx] : '\0';
}
void advancen(Lexer* lexer, size_t n) {
  lexer->idx = (lexer->idx + n < lexer->len) ? lexer->idx + n : lexer->len;
  lexer->current = lexer->input[lexer->idx];
}

char peek(Lexer lexer) {
  return (lexer.idx + 1 < lexer.len) ? lexer.input[lexer.idx + 1] : '\0';
}

void skip_whitespace(Lexer* lexer) {
  while (isspace(lexer->current)) advance(lexer);
}

// IDENTIFIER: command names, args, paths
Token read_identifier(Lexer* lexer) {
  size_t cap = 32;
  size_t len = 0;
  char* buffer = malloc(cap);
  // if (!buffer) exit(1);

  while (lexer->current && !isspace((unsigned char)lexer->current) &&
         lexer->current != '<' && lexer->current != '>' &&
         lexer->current != '|') {
    if (len + 1 >= cap) {
      cap *= 2;
      // char* tmp = realloc(buffer, cap);
      // if (!tmp) {
      //   free(buffer);
      //   exit(1);
      // }
      // buffer = tmp;
      buffer = realloc(buffer, cap);
    }

    buffer[len++] = lexer->current;
    advance(lexer);
  }

  buffer[len] = '\0';

  Token tok = new_tok(IDENTIFIER, buffer, lexer->idx);
  free(buffer);  // safe if new_tok copies string
  return tok;
}

// STRING: quoted arguments
Token read_string(Lexer* lexer) {
  char quote = lexer->current;
  advance(lexer);

  char buffer[1024];
  size_t i = 0;

  while (lexer->current != quote && !is_eof(lexer) && i < sizeof(buffer) - 1) {
    if (lexer->current == '\\') {
      advance(lexer);
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

  if (lexer->current == quote) advance(lexer);
  buffer[i] = '\0';

  return new_tok(STRING, buffer, lexer->idx);
}

// Symbols: < > >> | &
Token read_symbol(Lexer* lexer) {
  size_t start = lexer->idx;

  switch (lexer->current) {
    case '<':
      advance(lexer);
      return new_tok(STDIN_REDIRECT, "<", start);

    case '>':
      if (peek(*lexer) == '>') {
        advancen(lexer, 2);
        return new_tok(REDIRECT_APPEND, ">>", start);
      }
      advance(lexer);
      return new_tok(STDOUT_REDIRECT, ">", start);

    case '|':
      advance(lexer);
      return new_tok(PIPE, "|", start);

    case '&':
      advance(lexer);
      return new_tok(BACKGROUND, "&", start);

    default:
      advance(lexer);
      return new_tok(UNKNOWN, (char[]){lexer->current, '\0'}, start);
  }
}

Token next_token(Lexer* lexer) {
  skip_whitespace(lexer);

  if (is_eof(lexer)) return new_tok(END_OF_FILE, "", lexer->idx);

  if (lexer->current == '\'' || lexer->current == '"')
    return read_string(lexer);

  if (is_ident_char(lexer->current)) return read_identifier(lexer);

  return read_symbol(lexer);
}

Tokens Lex(const char* src) {
  Lexer lexer = init_lexer(src);
  Tokens toks = {0};
  toks.capacity = 16;
  toks.items = malloc(toks.capacity * sizeof(*toks.items));

  while (true) {
    Token token = next_token(&lexer);
    if (toks.count >= toks.capacity) {
      toks.capacity *= 2;
      toks.items = realloc(toks.items, toks.capacity * sizeof(*toks.items));
    }

    toks.items[toks.count].idx = token.idx;
    toks.items[toks.count].type = token.type;
    toks.items[toks.count++].lexeme = token.lexeme;

    if (toks.items[toks.count - 1].type == END_OF_FILE) break;
  }

  return toks;
}
