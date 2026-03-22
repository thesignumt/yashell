#pragma once

#include "cache.h"

char *read_input(void);
char *prompt_and_read(void);
void process_input(CmdCache *cc, char *input);

void run_shell(void);
