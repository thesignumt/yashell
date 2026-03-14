#include <string.h>

#include "cache.h"
#include "commands.h"
#define INIT_CC_CAPACITY 64  // initial cmd cache capacity

CmdResult cmd_echo(const char* args) {
  CmdResult result;
  result.data = NULL;
  result.output = strdup(args ? args : "");
  result.status = result.output ? STATUS_SUCCESS : STATUS_ERROR;
  return result;
}

CmdCache* cache_cmd_init(void) {
  CmdCache* cache = cmd_cache_init(INIT_CC_CAPACITY);

  cmd_cache_put(cache, "echo", cmd_echo);

  return cache;
}
