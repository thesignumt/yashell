#include <string.h>

#include "cache.h"
#include "commands.h"

CmdResult cmd_echo(const char* args) {
  CmdResult res;
  res.data = NULL;
  res.output = strdup(args ? args : "");
  res.status = res.output ? STATUS_SUCCESS : STATUS_ERROR;
  return res;
}

CmdResult cmd_exit(const char* args) {
  (void)args;
  return (CmdResult){STATUS_EXIT_CMD, NULL, NULL};
}

CmdCache* cache_cmd_init(void) {
  CmdCache* cache = cmd_cache_init(INIT_CC_CAPACITY);

  cmd_cache_put(cache, "echo", cmd_echo);
  cmd_cache_put(cache, "exit", cmd_exit);

  return cache;
}
