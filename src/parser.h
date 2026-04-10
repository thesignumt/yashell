#ifndef PARSER_H_
#define PARSER_H_

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>

#include "lexer.h"
#include "token.h"

typedef struct {
  char *name;
  char **argv;
  int argc;
  char *stdin_redirect;
  char *stdout_redirect;
  bool append_stdout;
} Cmd;

typedef struct {
  Cmd *cmds;
  int count;
  bool run_in_bg;
} Pipeline;

void add_cmd(Pipeline *pipeline, Cmd cmd);
void add_arg(Cmd *cmd, const char *arg);

void free_cmd(Cmd *cmd);
void free_pipeline(Pipeline *pipeline);

Pipeline *Parse(Tokens *tokens);

typedef enum {
  ARG_PATH,     // filesystem path
  ARG_LITERAL,  // plain arguments / words
  ARG_OPTION,   // e.g. -v, --help
  ARG_UNKNOWN   // anything else
} ArgType;

static inline ArgType validate_arg(const char *lexeme) {
  if (!lexeme || is_eof_char(lexeme[0])) return ARG_UNKNOWN;

  // Check if it's a path (contains / or starts with . or ~)
  if (strchr(lexeme, '/') || lexeme[0] == '.' || lexeme[0] == '~') {
    return ARG_PATH;
  }

  // Check if it's an option (starts with -)
  if (lexeme[0] == '-') return ARG_OPTION;

  // Check if it's a literal (alphanumeric or _)
  if (isalpha(lexeme[0]) || lexeme[0] == '_') {
    for (size_t i = 1; i < strlen(lexeme); i++) {
      if (!isalnum(lexeme[i]) && lexeme[i] != '_') {
        return ARG_UNKNOWN;
      }
    }
    return ARG_LITERAL;
  }

  return ARG_UNKNOWN;
}

#endif  // PARSER_H_
