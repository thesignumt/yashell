// TODO: support multiline prompt

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "token.h"

void print_cmd(const Cmd *cmd) {
  if (!cmd) return;
  printf("  Command:\n");
  printf("    Name: %s\n", cmd->name ? cmd->name : "(null)");

  printf("    Args:");
  if (cmd->args) {
    for (int i = 0; cmd->args[i]; i++) {
      printf(" %s", cmd->args[i]);
    }
    printf("\n");
  } else {
    printf(" (null)\n");
  }

  printf("    stdin_redirect: %s\n",
         cmd->stdin_redirect ? cmd->stdin_redirect : "(none)");
  printf("    stdout_redirect: %s%s\n",
         cmd->stdout_redirect ? cmd->stdout_redirect : "(none)",
         cmd->append_stdout ? " (append)" : "");
}

void print_pipeline(const Pipeline *pl) {
  if (!pl) return;
  printf("Pipeline (count=%d, run_in_bg=%s):\n", pl->count,
         pl->run_in_bg ? "true" : "false");
  for (int i = 0; i < pl->count; i++) {
    print_cmd(&pl->cmds[i]);
  }
}

int main(void) {
  bool yashell_running = true;
  char input[2049];
  while (yashell_running) {
    printf("> ");
    if (!fgets(input, sizeof(input), stdin)) {
      continue;
    }
    input[strcspn(input, "\n")] = '\0';

    TokenArr tokens = Lex(input);

    Pipeline *pipeline = Parse(&tokens);
    print_pipeline(pipeline);
  }

  return 0;
}
