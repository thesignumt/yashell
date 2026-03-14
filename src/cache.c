#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "commands.h"
#include "hashmap.h"

struct CmdCache {
  struct hashmap_s* map;
  size_t capacity;
};

CmdCache* cmd_cache_init(size_t capacity) {
  CmdCache* cache = malloc(sizeof(CmdCache));
  cache->capacity = capacity;

  cache->map = malloc(sizeof(struct hashmap_s));
  hashmap_create(capacity, cache->map);
  return cache;
}

static int free_cmdfns(void* ctx, struct hashmap_element_s* e) {
  (void)ctx;

  if (e->data) {
    free(e->data);
    e->data = NULL;
  }
  return -1;
}
void cmd_cache_free(CmdCache* cache) {
  if (!cache) return;

  hashmap_iterate_pairs(cache->map, free_cmdfns, NULL);  // free all CmdFn*
  hashmap_destroy(cache->map);  // destroy hashmap internals

  free(cache->map);  // free hashmap struct
  free(cache);       // free CmdCache
}

void cmd_cache_put(CmdCache* cache, const char* cmd_name, CmdFn f) {
  CmdFn* fp = malloc(sizeof(CmdFn));
  *fp = f;
  hashmap_put(cache->map, cmd_name, strlen(cmd_name), fp);
}

static CmdResult cmd_not_found(const char* args) {
  (void)args;
  return (CmdResult){STATUS_CMD_NOT_FOUND, NULL, NULL};
}
CmdFn cmd_cache_get(CmdCache* cache, const char* cmd_name) {
  CmdFn* fp = hashmap_get(cache->map, cmd_name, strlen(cmd_name));
  return fp ? *fp : cmd_not_found;
}
