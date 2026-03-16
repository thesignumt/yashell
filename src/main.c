// TODO: support multiline prompt

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "commands.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"

#define INPUT_SIZE 2048

int main(void) {
  bool yashell_running = true;
  CmdCache *cc = new_cc();

  char input[INPUT_SIZE];
  while (yashell_running) {
    printf(">>> ");
    if (!fgets(input, sizeof(input), stdin)) {
      break;
    }

    char *nl = strchr(input, '\n');
    if (nl) *nl = '\0';
    if (!*input) continue;

    Tokens tokens = Lex(input);
    Pipeline *pipeline = Parse(&tokens);
    if (pipeline->count == 0) continue;

    Cmd *cmd0 = &pipeline->cmds[0];
    CmdFn cmd_fn = cmd_cache_get(cc, cmd0->name);
    CmdResult res = cmd_fn(cmd0->argc, cmd0->argv);

    switch (res.status) {
      case STATUS_SUCCESS:
        if (res.output) printf("%s\n", res.output);
        break;
      case STATUS_ERROR:
        fputs("error has taken place! (⊙_⊙)？", stderr);
        continue;
      case STATUS_EXIT_CMD:
        cmd_cache_free(cc);
        yashell_running = false;
        break;
      case STATUS_CMD_NOT_FOUND:
        fprintf(stderr, "Command not found: %s\n", cmd0->name);
        continue;
    }
    if (res.output) free(res.output);
  }

  return 0;
}
