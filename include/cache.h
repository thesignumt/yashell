#ifndef CACHE_H_
#define CACHE_H_

#include "commands.h"
#include <stddef.h>

typedef struct CmdCache CmdCache;

CmdCache *cmd_cache_init(size_t capacity);
void cmd_cache_free(CmdCache *cache);

void cmd_cache_put(CmdCache *cache, const char *cmd_name, CmdFn f);
CmdFn cmd_cache_get(CmdCache *cache, const char *cmd_name);

#endif  // CACHE_H_
