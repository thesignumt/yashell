// TODO: make aliases (including pwd and more)

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>  // for _getcwd
#define getcwd _getcwd
#else
#include <limits.h>
#include <unistd.h>  // for getcwd
#endif

#include "cache.h"
#include "commands.h"

CmdResult cmd_cwd(int argc, char** argv) {
  (void)argc;
  (void)argv;

  CmdResult res;
  res.data = NULL;

  char* cwd = malloc(PATH_MAX);
  if (!cwd) return (CmdResult){STATUS_ERROR, "malloc failed", NULL};

  if (getcwd(cwd, PATH_MAX) != NULL) {
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
    res.status = res.output ? STATUS_SUCCESS : STATUS_ERROR;
    res.output = strdup("");
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
      {"cwd", cmd_cwd},
      {"pwd", cmd_cwd},  // alias
      {"echo", cmd_echo},
      {"exit", cmd_exit},
  };

  for (size_t i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++)
    cmd_cache_put(cache, cmds[i].name, cmds[i].f);

  return cache;
}
