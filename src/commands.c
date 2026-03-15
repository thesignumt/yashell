#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "commands.h"

CmdResult cmd_echo(int argc, char** argv) {
  CmdResult res;
  res.data = NULL;

  if (argc == 0) {
    res.output = strdup("");
    res.status = res.output ? STATUS_SUCCESS : STATUS_ERROR;
    return res;
  }

  size_t len = 0;
  for (int i = 0; i < argc; i++) len += strlen(argv[i]);
  len += (argc > 1 ? argc - 1 : 0) + 1;  // spaces + null terminator

  char* out = malloc(len);
  if (!out) {
    res.status = STATUS_ERROR;
    res.output = NULL;
    return res;
  }

  char* p = out;
  for (int i = 0; i < argc; i++) {
    size_t l = strlen(argv[i]);
    memcpy(p, argv[i], l);
    p += l;
    if (i < argc - 1) *p++ = ' ';
  }
  *p = '\0';

  res.output = out;
  res.status = STATUS_SUCCESS;
  return res;
}

CmdResult cmd_exit(int argc, char** argv) {
  (void)argc;
  (void)argv;
  return (CmdResult){STATUS_EXIT_CMD, NULL, NULL};
}

/**
 * @brief new commands cache/registry
 *
 * @return freshly made cmd cache
 */
CmdCache* new_cc(void) {
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
