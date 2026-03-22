#pragma once

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INIT_CC_CAPACITY 64 // initial cmd cache capacity
#define DATE_BUFFER 100
#define CAT_BUF_SIZE 65536

typedef struct CmdCache CmdCache;

typedef enum {
  STATUS_SUCCESS = 0,
  STATUS_ERROR = 1,
  STATUS_EXIT_CMD = 2,
  STATUS_CMD_NOT_FOUND = 3
  // TODO: STATUS_CTRL_C = 4
} CmdStatus;

typedef struct {
  CmdStatus status; // 0 = success, non-zero = error code
  char *output;     // optional string output from the command
  void *data;       // optional pointer to extra data (if needed)
} CmdResult;

typedef CmdResult (*CmdFn)(int argc, char *argv[]);

CmdResult cmd_clear(int argc, char *argv[]);
CmdResult cmd_cwd(int argc, char *argv[]);
CmdResult cmd_date(int argc, char *argv[]);
CmdResult cmd_echo(int argc, char *argv[]);
CmdResult cmd_exit(int argc, char *argv[]);
CmdResult cmd_true(int argc, char *argv[]);
CmdResult cmd_false(int argc, char *argv[]);
CmdResult cmd_whoami(int argc, char *argv[]);

////////////////////////////////////////////////////////////

static inline CmdResult ok(char *output) {
  return (CmdResult){STATUS_SUCCESS, output, NULL};
}

static inline CmdResult ok_void(void) {
  return (CmdResult){STATUS_SUCCESS, NULL, NULL};
}

static inline CmdResult err(const char *msg) {
  return (CmdResult){STATUS_ERROR, msg ? strdup(msg) : NULL, NULL};
}

static inline CmdResult err_from_errno(void) { return err(strerror(errno)); }

static inline CmdResult oom(void) { return err("malloc failed"); }

static inline char *xstrdup(const char *s) {
  if (!s)
    return NULL;
  return strdup(s);
}

static inline char *getenv_dup(const char *name) {
  const char *val = getenv(name);
  return val ? strdup(val) : NULL;
}

static inline char *format_time(const char *fmt) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  if (!t)
    return NULL;

  char buf[DATE_BUFFER];
  if (!strftime(buf, sizeof(buf), fmt, t))
    return NULL;

  return strdup(buf);
}

////////////////////////////////////////////////////////////

CmdCache *new_cc(void);
