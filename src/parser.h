#ifndef PARSER_H_
#define PARSER_H_

#include <stdbool.h>
#include <stddef.h>

#include "token.h"

typedef struct {
    char **argv; // argv[0] = command name
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

bool add_cmd(Pipeline *pipeline, Cmd cmd);
bool add_arg(Cmd *cmd, const char *arg);

void free_cmd(Cmd *cmd);
void free_pipeline(Pipeline *pipeline);

Pipeline *Parse(Tokens *tokens);

#endif // PARSER_H_
