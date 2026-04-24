#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "parser.h"
#include "token.h"

bool add_cmd(Pipeline *pipeline, Cmd cmd) {
    Cmd *tmp = realloc(pipeline->cmds, (pipeline->count + 1) * sizeof(*tmp));
    if (!tmp)
        return false;

    pipeline->cmds = tmp;
    pipeline->cmds[pipeline->count++] = cmd;
    return true;
}
bool add_arg(Cmd *cmd, const char *arg) {
    char **tmp = realloc(cmd->argv, sizeof(*cmd->argv) * (cmd->argc + 2));
    if (!tmp)
        return false;

    char *dup = xstrdup(arg);
    if (!dup)
        return false;

    cmd->argv = tmp;
    cmd->argv[cmd->argc++] = dup;
    cmd->argv[cmd->argc] = NULL;

    return true;
}

void free_cmd(Cmd *cmd) {
    if (!cmd)
        return;

    if (cmd->argv) {
        for (int i = 0; i < cmd->argc; i++)
            free(cmd->argv[i]);

        free(cmd->argv);
    }

    free(cmd->stdin_redirect);
    free(cmd->stdout_redirect);
}
void free_pipeline(Pipeline *pipeline) {
    if (!pipeline)
        return;

    for (int i = 0; i < pipeline->count; i++)
        free_cmd(&pipeline->cmds[i]);

    free(pipeline->cmds);
    free(pipeline);
}

Pipeline *Parse(Tokens *tokens) {
    Pipeline *pipeline = calloc(1, sizeof(Pipeline));
    if (!pipeline)
        return NULL;
    Cmd cmd = {0};

    size_t i = 0;
    while (i < tokens->count) {
        Token token = tokens->items[i];

        switch (token.type) {
        case PIPE:
            if (!add_cmd(pipeline, cmd))
                goto fail;
            cmd = (Cmd){0};
            break;

        case STDIN_REDIRECT:
            if (++i < tokens->count)
                cmd.stdin_redirect = xstrdup(tokens->items[i].lexeme);
            break;

        case STDOUT_REDIRECT:
            if (++i < tokens->count)
                cmd.stdout_redirect = xstrdup(tokens->items[i].lexeme);
            cmd.append_stdout = false;
            break;

        case REDIRECT_APPEND:
            if (++i < tokens->count)
                cmd.stdout_redirect = xstrdup(tokens->items[i].lexeme);
            cmd.append_stdout = true;
            break;

        case BACKGROUND:
            pipeline->run_in_bg = true;
            break;

        case END_OF_FILE:
            if (cmd.argc > 0)
                if (!add_cmd(pipeline, cmd))
                    goto fail;

            return pipeline;

        default:
            if (token.type == IDENTIFIER || token.type == STRING)
                if (!add_arg(&cmd, token.lexeme))
                    goto fail;

            break;
        }

        i++;
    }

    if (cmd.argc > 0)
        if (!add_cmd(pipeline, cmd))
            goto fail;

    return pipeline;

fail:
    free_pipeline(pipeline);
    return NULL;
}
