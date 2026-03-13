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
      continue;
    }

    char *nl = strchr(input, '\n');
    if (nl) *nl = '\0';

    TokenArr tokens = Lex(input);

    Pipeline *pipeline = Parse(&tokens);
    print_pipeline(pipeline);
  }

  return 0;
}
