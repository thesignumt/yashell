// TODO: support multiline prompt

#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "commands.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"

const char *get_home_cached(void) {
#ifdef _WIN32
    const char *h = getenv("USERPROFILE");
    return h ? h : getenv("HOMEDRIVE");
#else
    return getenv("HOME");
#endif
}

char *read_input(void) {
    size_t size = 2048;
    char *buf = malloc(size);
    if (!buf)
        return NULL;

    if (!fgets(buf, size, stdin)) {
        free(buf);
        return NULL;
    }

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
        buf[len - 1] = '\0';

    return buf;
}

char *prompt_and_read(void) {
    char *cwd = _getcwd(NULL, 0);
    const char *home = get_home_cached();

    if (cwd) {
        if (home && strncmp(cwd, home, strlen(home)) == 0)
            printf("~%s $ ", cwd + strlen(home));
        else
            printf("%s $ ", cwd);

        free(cwd);
    } else
        fprintf(stderr, "Unable to get current directory. $ ");
    fflush(stdout);

    return read_input();
}

void process_input(CmdCache *cc, char *input) {
    if (!input || !*input)
        return;

    Tokens tokens = Lex(input);
    Pipeline *pipeline = Parse(&tokens);
    if (!pipeline || pipeline->count == 0) {
        free_tokens(&tokens);
        free_pipeline(pipeline);
        return;
    }

    Cmd *cmd0 = &pipeline->cmds[0];
    CmdFn cmd_fn = cmd_cache_get(cc, cmd0->argv[0]);
    CmdResult res = cmd_fn(cmd0->argc, cmd0->argv);

    switch (res.status) {
    case STATUS_SUCCESS:
        if (res.output)
            puts(res.output);
        break;
    case STATUS_ERROR:
        if (res.output)
            fprintf(stderr, "Error: %s\n", res.output);
        break;
    case STATUS_EXIT_CMD:
        free(input);
        free_tokens(&tokens);
        free_pipeline(pipeline);
        cmd_cache_free(cc);
        exit(0);
    case STATUS_CMD_NOT_FOUND:
        fprintf(stderr, "Command not found: %s\n", cmd0->argv[0]);
        break;

    case STATUS_UNSET:
        break;
    }

    if (res.output)
        free(res.output);
    free_tokens(&tokens);
    free_pipeline(pipeline);
}

void run_shell(void) {
    CmdCache *cc = new_cc();
    char *input;
    while ((input = prompt_and_read()) != NULL) {
        process_input(cc, input);
    }
    cmd_cache_free(cc);
}

int main(void) {
    run_shell();
    return 0;
}
