// TODO: make aliases (including pwd and more)

#include <direct.h>  // for _getcwd
#include <errno.h>
#include <limits.h>  // for _MAX_PATH
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cache.h"
#include "commands.h"

#define DATE_BUFFER 100

CmdResult cmd_clear(int argc, char** argv) {
  (void)argc;
  (void)argv;
  printf("\033[H\033[J");
  return (CmdResult){STATUS_SUCCESS, NULL, NULL};
}

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

CmdResult cmd_date(int argc, char** argv) {
  (void)argc;
  (void)argv;
  CmdResult res;
  res.data = NULL;

  time_t now;
  struct tm* t;
  char buffer[DATE_BUFFER];

  time(&now);
  t = localtime(&now);

  strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Z %Y", t);

  res.output = strdup(buffer);

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

CmdResult cmd_whoami(int argc, char** argv) {
  (void)argc;
  (void)argv;
  CmdResult res = {0};

  char* user = getenv("USER");           // Linux/macOS
  if (!user) user = getenv("USERNAME");  // Windows fallback

  if (user) {
    res.status = STATUS_SUCCESS;
    res.output = strdup(user);
  } else {
    res.status = STATUS_ERROR;
    res.output = strdup("unknown username");
  }
  return res;
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
  } cmds[] = {{"clear", cmd_clear},  {"cls", cmd_clear},   {"cwd", cmd_cwd},
              {"date", cmd_date},    {"pwd", cmd_cwd},     {"echo", cmd_echo},
              {"exit", cmd_exit},    {"false", cmd_false}, {"true", cmd_true},
              {"whoami", cmd_whoami}};

  for (size_t i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++)
    cmd_cache_put(cache, cmds[i].name, cmds[i].f);

  return cache;
}
