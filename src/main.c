// TODO: support multiline prompt

#include <direct.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "commands.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"

char *read_input(void) {
  size_t size = 2048;
  size_t len = 0;
  char *buf = malloc(size);
  if (!buf) return NULL;

  while (fgets(buf + len, size - len, stdin)) {
    len += strlen(buf + len);
    if (len > 0 && buf[len - 1] == '\n') break;
    size *= 2;
    char *tmp = realloc(buf, size);
    if (!tmp) {
      free(buf);
      return NULL;
    }
    buf = tmp;
  }

  if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
  return buf;
}

int main(void) {
  char cwd[PATH_MAX];

  for (CmdCache *cc = new_cc();;) {
    if (getcwd(cwd, sizeof(cwd)) != NULL)
      printf("%s $ ", cwd);
    else
      fprintf(stderr, "Unable to get current directory. $ ");
    fflush(stdout);

    char *input = read_input();
    if (!input) break;

    if (!*input) {
      free(input);
      continue;
    }

    Tokens tokens = Lex(input);
    Pipeline *pipeline = Parse(&tokens);
    if (!pipeline || pipeline->count == 0) {
      free_tokens(&tokens);
      free_pipeline(pipeline);
      free(input);
      continue;
    }

    Cmd *cmd0 = &pipeline->cmds[0];
    CmdFn cmd_fn = cmd_cache_get(cc, cmd0->name);
    CmdResult res = cmd_fn(cmd0->argc, cmd0->argv);

    switch (res.status) {
      case STATUS_SUCCESS:
        if (res.output) puts(res.output);
        break;
      case STATUS_ERROR:
        if (res.output) fprintf(stderr, "Error: %s\n", res.output);
        break;
      case STATUS_EXIT_CMD:
        cmd_cache_free(cc);
        return 0;
      case STATUS_CMD_NOT_FOUND:
        fprintf(stderr, "Command not found: %s\n", cmd0->name);
        break;
    }

    // free memory
    if (res.output) free(res.output);
    free_tokens(&tokens);
    free_pipeline(pipeline);
    free(input);
  }

  return 0;
}
