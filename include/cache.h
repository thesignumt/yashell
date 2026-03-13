#pragma once

#include "commands.h"
#include <stddef.h>

typedef CmdResult (*CmdFn)(const char *args);
typedef struct CmdCache CmdCache;

CmdCache *cmd_cache_init(size_t capacity);
void cmd_cache_free(CmdCache *cache);

void cmd_cache_put(CmdCache *cache, const char *cmd_name, CmdFn f);
CmdFn cmd_cache_get(CmdCache *cache, const char *cmd_name);
