#pragma once

#include "token.h"
#include <stdbool.h>
#include <stddef.h>

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

Pipeline *Parse(TokenArr *tokens);
