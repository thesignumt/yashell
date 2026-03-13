#include <string.h>

#include "cache.h"
#include "commands.h"

CmdResult cmd_echo(const char* args) {
  CmdResult result = {0, NULL, NULL};
  result.output = strdup(args ? args : "");
  result.status = result.output ? STATUS_SUCCESS : STATUS_ERROR;
  return result;
}

CmdCache* cache_cmd_init(void) {
  CmdCache* cache = cmd_cache_init(64);

  cmd_cache_put(cache, "echo", cmd_echo);

  return cache;
}
