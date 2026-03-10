#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "token.h"

void add_cmd(Pipeline* pipeline, Cmd cmd) {
  pipeline->cmds = realloc(pipeline->cmds, (pipeline->count + 1) * sizeof(Cmd));
  pipeline->cmds[pipeline->count] = cmd;
  pipeline->count++;
}
void add_arg(Cmd* cmd, const char* arg) {
  int n = 0;
  if (cmd->args) {
    while (cmd->args[n]) n++;  // count existing args
  }

  // realloc to hold one more arg + NULL terminator
  cmd->args = realloc(cmd->args, (n + 2) * sizeof(char*));

  // allocate and copy the new string
  cmd->args[n] = malloc(strlen(arg) + 1);
  strcpy(cmd->args[n], arg);

  cmd->args[n + 1] = NULL;  // keep NULL-terminated
}

Pipeline* Parse(TokenArr* tokens) {
  Pipeline* pipeline = calloc(1, sizeof(Pipeline));
  Cmd cmd = {0};

  size_t i = 0;
  while (i < tokens->count) {
    Token token = tokens->tokens[i];

    if (token.type == PIPE) {
      add_cmd(pipeline, cmd);
      cmd = (Cmd){0};
    } else if (token.type == STDIN_REDIRECT) {
      i++;
        cmd.stdin_redirect = strdup(tokens->tokens[i].lexeme);
    } else if (token.type == STDOUT_REDIRECT) {
      i++;
        cmd.stdout_redirect = strdup(tokens->tokens[i].lexeme);
      cmd.append_stdout = false;
    } else if (token.type == REDIRECT_APPEND) {
      i++;
        cmd.stdout_redirect = strdup(tokens->tokens[i].lexeme);
      cmd.append_stdout = true;
    } else if (token.type == BACKGROUND) {
      pipeline->run_in_bg = true;
    } else {
      if (cmd.name == NULL || cmd.name[0] == '\0') {
        cmd.name = strdup(token.lexeme);
      } else {
        add_arg(&cmd, token.lexeme);
      }
    }

    i++;
  }

  add_cmd(pipeline, cmd);
  return pipeline;
}
