#pragma once

#include "cache.h"
#include <stdio.h>

static inline void handle_sigint(int sig) {
  (void)sig;      // unused
  printf("\n");   // move to new line
  fflush(stdout); // show prompt again
}

char *read_input(void);
char *prompt_and_read(void);
void process_input(CmdCache *cc, char *input);

void run_shell(void);
