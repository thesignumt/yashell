#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "token.h"

Lexer init_lexer(const char* input) {
  Lexer lexer;
  lexer.input = input;
  lexer.idx = 0;
  lexer.current = input[0];
  return lexer;
}

void advance(Lexer* lexer) {
  lexer->idx += 1;
  lexer->current = lexer->input[lexer->idx];
}
void advancen(Lexer* lexer, size_t n) {
  size_t remaining =
      strlen(lexer->input + lexer->idx);        // chars left including current
  size_t step = n < remaining ? n : remaining;  // don't go past end
  lexer->idx += step;
  lexer->current = lexer->input[lexer->idx];
}

char peek(Lexer lexer) {
  return lexer.input[lexer.idx + 1] ? lexer.input[lexer.idx + 1] : '\0';
}
char peekn(Lexer lexer, size_t n) {
  size_t idx = lexer.idx + n;
  return lexer.input[idx] ? lexer.input[idx] : '\0';
}

bool is_eof(Lexer* lexer) { return lexer->current == '\0'; }

void skip_whitespace(Lexer* lexer) {
  while (isspace(lexer->current)) advance(lexer);
}

// IDENTIFIER: command names, args, paths
Token* read_identifier(Lexer* lexer) {
  char buffer[1024];
  size_t i = 0;
  while (lexer->current != '\0' && !isspace(lexer->current) &&
         lexer->current != '<' && lexer->current != '>' &&
         lexer->current != '|' && i < sizeof(buffer) - 1) {
    buffer[i++] = lexer->current;
    advance(lexer);
  }
  buffer[i] = '\0';
  return new_tok(IDENTIFIER, buffer, lexer->idx);
}

// STRING: quoted arguments
Token* read_string(Lexer* lexer) {
  char quote = lexer->current;
  advance(lexer);  // skip opening quote
  char buffer[1024];
  size_t i = 0;

  while (lexer->current != quote && !is_eof(lexer) && i < sizeof(buffer) - 1) {
    if (lexer->current == '\\') {  // escape
      advance(lexer);
      if (lexer->current == quote || lexer->current == '\\') {
        buffer[i++] = lexer->current;
        advance(lexer);
      } else {
        buffer[i++] = '\\';
      }
    } else {
      buffer[i++] = lexer->current;
      advance(lexer);
    }
  }
  advance(lexer);  // skip closing quote
  buffer[i] = '\0';
  return new_tok(STRING, buffer, lexer->idx);
}

// Symbols: < > >> | &
Token* read_symbol(Lexer* lexer) {
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
      return NULL;  // unknown symbol
  }
}

Token* next_token(Lexer* lexer) {
  skip_whitespace(lexer);
  if (is_eof(lexer)) return new_tok(END_OF_FILE, "", lexer->idx);

  if (lexer->current == '\'' || lexer->current == '"')
    return read_string(lexer);
  if (isalnum(lexer->current) || strchr("./-_", lexer->current))
    return read_identifier(lexer);

  return read_symbol(lexer);
}

TokenArr Lex(const char* src) {
  Lexer lexer = init_lexer(src);
  size_t capacity = 8;
  size_t count = 0;
  Token* tokens = malloc(capacity * sizeof(Token));

  bool lexing = true;
  while (lexing) {
    Token* token = next_token(&lexer);

    if (count >= capacity) {
      capacity *= 2;
      tokens = realloc(tokens, capacity * sizeof(Token));
    }

    tokens[count].type = token->type;
    tokens[count].lexeme = strdup(token->lexeme);
    tokens[count].idx = token->idx;
    count++;

    if (token->type == END_OF_FILE) lexing = false;

    free(token->lexeme);
    free(token);
  }

  TokenArr result = {.tokens = tokens, .count = count};
  return result;
}
