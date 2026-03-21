// TODO: support multiline prompt

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
  char input[INPUT_SIZE];
  for (CmdCache *cc = new_cc();;) {
    printf("$ ");
    if (!fgets(input, sizeof(input), stdin)) break;

    // flush remaining input if line too long
    if (strchr(input, '\n') == NULL) {
      int c;
      while ((c = getchar()) != '\n' && c != EOF);  // flush
    }

    char *p = input;
    while (*p && *p != '\n') p++;
    *p = '\0';
    if (!*input) continue;

    Tokens tokens = Lex(input);
    Pipeline *pipeline = Parse(&tokens);
    free_tokens(&tokens);
    if (!pipeline || pipeline->count == 0) continue;

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
    free_pipeline(pipeline);
  }

  return 0;
}
