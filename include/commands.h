#pragma once

#define INIT_CC_CAPACITY 64 // initial cmd cache capacity

typedef struct CmdCache CmdCache;

typedef enum {
  STATUS_SUCCESS = 0,
  STATUS_ERROR = 1,
  STATUS_EXIT_CMD = 2,
  STATUS_CMD_NOT_FOUND = 3
} CmdStatus;

typedef struct {
  CmdStatus status; // 0 = success, non-zero = error code
  char *output;     // optional string output from the command
  void *data;       // optional pointer to extra data (if needed)
} CmdResult;

typedef CmdResult (*CmdFn)(const char *args);
CmdResult cmd_echo(const char *args);

CmdCache *cache_cmd_init(void);
