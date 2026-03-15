#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "commands.h"

CmdResult cmd_echo(int argc, const char* args[]) {
  CmdResult res;
  res.data = NULL;

  if (argc == 0) {
    res.output = strdup("");
    res.status = res.output ? STATUS_SUCCESS : STATUS_ERROR;
    return res;
  }
  size_t len = 1;
  for (int i = 0; i < argc; i++)
    len += strlen(args[i]) + 1;  // +1 for space/null

  char* out = malloc(len);
  if (!out) {
    res.status = STATUS_ERROR;
    res.output = NULL;
    return res;
  }

  out[0] = '\0';
  for (int i = 0; i < argc; i++) {
    strcat(out, args[i]);
    if (i != argc - 1) strcat(out, " ");
  }
  res.output = out;
  res.status = STATUS_SUCCESS;
  return res;
}

CmdResult cmd_exit(int argc, const char* args[]) {
  (void)argc;
  (void)args;
  return (CmdResult){STATUS_EXIT_CMD, NULL, NULL};
}

CmdCache* cache_cmd_init(void) {
  CmdCache* cache = cmd_cache_init(INIT_CC_CAPACITY);

  static struct {
    const char* name;
    CmdFn f;
  } cmds[] = {
      {"echo", cmd_echo},
      {"exit", cmd_exit},
  };

  for (size_t i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++)
    cmd_cache_put(cache, cmds[i].name, cmds[i].f);

  return cache;
}
