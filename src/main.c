// TODO: support multiline prompt

#include <direct.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "commands.h"
#include "lexer.h"
#include "main.h"
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

char *prompt_and_read(void) {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL)
    printf("%s $ ", cwd);
  else
    fprintf(stderr, "Unable to get current directory. $ ");
  fflush(stdout);

  return read_input();
}

void process_input(CmdCache *cc, char *input) {
  if (!input || !*input) return;

  Tokens tokens = Lex(input);
  Pipeline *pipeline = Parse(&tokens);
  if (!pipeline || pipeline->count == 0) {
    free_tokens(&tokens);
    free_pipeline(pipeline);
    return;
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
      free(input);
      free_tokens(&tokens);
      free_pipeline(pipeline);
      cmd_cache_free(cc);
      exit(0);
    case STATUS_CMD_NOT_FOUND:
      fprintf(stderr, "Command not found: %s\n", cmd0->name);
      break;
  }

  if (res.output) free(res.output);
  free_tokens(&tokens);
  free_pipeline(pipeline);
}

void run_shell(void) {
  CmdCache *cc = new_cc();
  for (;;) {
    char *input = prompt_and_read();
    if (!input) break;
    process_input(cc, input);
    free(input);
  }
  cmd_cache_free(cc);
}

int main(void) {
  signal(SIGINT, handle_sigint);
  run_shell();
  return 0;
}
