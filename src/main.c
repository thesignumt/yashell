// TODO: support multiline prompt

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "token.h"

int main(void) {
  bool yashell_running = true;
  char input[2049];
  while (yashell_running) {
    printf(">>> ");
    if (!fgets(input, sizeof(input), stdin)) {
      break;
    }

    char *nl = strchr(input, '\n');
    if (nl) *nl = '\0';
    if (!*input) continue;

    TokenArr tokens = Lex(input);

    Pipeline *pipeline = Parse(&tokens);
    if (pipeline->count == 0) continue;

    Cmd *cmd0 = &pipeline->cmds[0];
    puts(cmd0->name);
  }

  return 0;
}
