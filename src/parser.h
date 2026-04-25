#ifndef PARSER_H_
#define PARSER_H_

#include <stddef.h>

#include "lexer.h"

typedef struct {
    char **argv; // argv[0] = command name
    int argc;

    char *stdin_redirect;
    char *stdout_redirect;
    int append_stdout;

    struct Cmd *next;
} Cmd;

typedef struct {
    Cmd *cmds;
    int count;
    int run_in_bg;
} Pipeline;

int add_cmd(Pipeline *pipeline, Cmd cmd);
int add_arg(Cmd *cmd, const char *arg);

void free_cmd(Cmd *cmd);
void free_pipeline(Pipeline *pipeline);

Pipeline *Parse(Tokens *tokens);

#endif // PARSER_H_
