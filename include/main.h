#pragma once

#include "cache.h"
#include "commands.h"

static inline char get_status_char(CmdStatus status) {
  switch (status) {
  case STATUS_SUCCESS:
    return '0';
  case STATUS_CMD_NOT_FOUND:
  case STATUS_ERROR:
    return '1';
  case STATUS_UNSET:
    return '_';
  default:
    return '?';
  }
}

char *read_input(void);
char *prompt_and_read(void);
void process_input(CmdCache *cc, char *input);

void run_shell(void);
