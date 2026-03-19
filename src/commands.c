// TODO: make aliases (including pwd and more)

#include <direct.h>  // for _getcwd
#include <errno.h>
#include <limits.h>  // for _MAX_PATH
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "commands.h"

CmdResult cmd_cwd(int argc, char** argv) {
  (void)argc;
  (void)argv;

  CmdResult res;
  res.data = NULL;

  char* cwd = malloc(_MAX_PATH);
  if (!cwd) return (CmdResult){STATUS_ERROR, "malloc failed", NULL};

  if (_getcwd(cwd, _MAX_PATH) != NULL) {
    res.status = STATUS_SUCCESS;
    res.output = cwd;
  } else {
    free(cwd);
    res.status = STATUS_ERROR;
    res.output = strdup(strerror(errno));
  }
  return res;
}

CmdResult cmd_echo(int argc, char** argv) {
  CmdResult res;
  res.data = NULL;

  if (argc == 0) {
    res.status = STATUS_SUCCESS;
    res.output = strdup("");
    if (!res.output) {
      res.status = STATUS_ERROR;
      res.output = strdup("malloc failed");
    }
    return res;
  }

  size_t len = 0;
  for (int i = 0; i < argc; i++) len += strlen(argv[i]);
  len += (argc > 1 ? argc - 1 : 0) + 1;

  char* out = malloc(len);
  if (!out) {
    res.status = STATUS_ERROR;
    res.output = strdup("malloc failed");
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

  res.status = STATUS_SUCCESS;
  res.output = out;
  return res;
}

CmdResult cmd_exit(int argc, char** argv) {
  (void)argc;
  (void)argv;
  return (CmdResult){STATUS_EXIT_CMD, NULL, NULL};
}

CmdResult cmd_true(int argc, char** argv) {
  (void)argc;
  (void)argv;
  return (CmdResult){STATUS_SUCCESS, NULL, NULL};
}
CmdResult cmd_false(int argc, char** argv) {
  (void)argc;
  (void)argv;
  return (CmdResult){STATUS_ERROR, NULL, NULL};
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
  } cmds[] = {{"cwd", cmd_cwd},   {"pwd", cmd_cwd},  // alias
              {"echo", cmd_echo}, {"exit", cmd_exit},
              {"true", cmd_true}, {"false", cmd_false}};

  for (size_t i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++)
    cmd_cache_put(cache, cmds[i].name, cmds[i].f);

  return cache;
}
