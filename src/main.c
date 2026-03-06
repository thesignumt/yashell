#include <stdbool.h>
#include <stdio.h>

int main() {
  bool yashell_running = true;
  char input[2049];
  while (yashell_running) {
    printf("> ");
    fgets(input, sizeof(input), stdin);
    printf("%s", input);
  }

  return 0;
}
