#include <stdlib.h>
#include <string.h>

#include "cache.h"
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

void cmd_cache_free(CmdCache* cache) {
  hashmap_destroy(cache->map);  // free the hashmap
  free(cache);
}

void cmd_cache_put(CmdCache* cache, const char* cmd_name, CmdFn f) {
  CmdFn* fp = malloc(sizeof(CmdFn));
  *fp = f;
  hashmap_put(cache->map, cmd_name, strlen(cmd_name), fp);
}

CmdFn cmd_cache_get(CmdCache* cache, const char* cmd_name) {
  CmdFn* fp = hashmap_get(cache->map, cmd_name, strlen(cmd_name));
  return fp ? *fp : NULL;
}
