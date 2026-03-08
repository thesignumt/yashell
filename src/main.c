// TODO: support multiline prompt

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "token.h"

int main(void) {
  bool yashell_running = true;
  char input[2049];
  while (yashell_running) {
    printf("> ");
    if (!fgets(input, sizeof(input), stdin)) {
      continue;
    }
    input[strcspn(input, "\n")] = '\0';

    size_t token_count;
    Token* tokens = Lex(input, &token_count);
    for (size_t i = 0; i < token_count; i++) {
      printf("Token %zu: type=%d, lexeme='%s', idx=%zu\n", i, tokens[i].type,
             tokens[i].lexeme, tokens[i].idx);
      free(tokens[i].lexeme);
    }

    free(tokens);
  }

  return 0;
}
