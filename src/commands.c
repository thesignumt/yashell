// TODO: make aliases (including pwd and more)

#include <direct.h>  // for _getcwd
#include <errno.h>
#include <fcntl.h>
#include <limits.h>  // for _MAX_PATH
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cache.h"
#include "commands.h"

CmdResult cmd_cat(int argc, char** argv) {
  if (argc < 1) return err("no file specified");

  const char* filename = argv[0];

  // use _open (on windows) with _O_BINARY to avoid CRLF translation
  int fd = _open(filename, _O_RDONLY | _O_BINARY);
  if (fd < 0) return err(strerror(errno));

  struct _stat st;
  if (_fstat(fd, &st) < 0) {
    _close(fd);
    return err(strerror(errno));
  }

  size_t size = st.st_size;
  char* buf = malloc(size + 1);  // +1 for '\0'
  if (!buf) {
    _close(fd);
    return err(strerror(errno));
  }

  size_t total = 0;
  ssize_t n;
  while (total < size &&
         (n = _read(
              fd, buf + total,
              CAT_BUF_SIZE < size - total ? CAT_BUF_SIZE : size - total)) > 0)
    total += n;

  _close(fd);

  if (total != size) {  // read error
    free(buf);
    return err(strerror(errno));
  }

  buf[total] = '\0';  // null-terminate
  return ok(buf);
}

CmdResult cmd_clear(int argc, char** argv) {
  (void)argc;
  (void)argv;
  printf("\033[H\033[J");
  return ok_void();
}

CmdResult cmd_cwd(int argc, char** argv) {
  (void)argc;
  (void)argv;

  char* cwd = malloc(_MAX_PATH);
  if (!cwd) return oom();

  if (_getcwd(cwd, _MAX_PATH) != NULL)
    return ok(cwd);
  else {
    free(cwd);
    return err(strerror(errno));
  }
}

CmdResult cmd_date(int argc, char** argv) {
  (void)argc;
  (void)argv;

  char* out = format_time("%a %b %d %H:%M:%S %Z %Y");
  if (!out) return err("time format failed");
  return ok(out);
}

CmdResult cmd_echo(int argc, char** argv) {
  if (argc == 0) {
    char* empty = xstrdup("");
    if (!empty) return oom();
    return ok(empty);
  }

  size_t len = 0;
  for (int i = 0; i < argc; i++) len += strlen(argv[i]);
  len += (argc > 1 ? argc - 1 : 0) + 1;

  char* out = malloc(len);
  if (!out) return oom();

  char* p = out;
  for (int i = 0; i < argc; i++) {
    size_t l = strlen(argv[i]);
    memcpy(p, argv[i], l);
    p += l;
    if (i < argc - 1) *p++ = ' ';
  }
  *p = '\0';

  return ok(out);
}

CmdResult cmd_exit(int argc, char** argv) {
  (void)argc;
  (void)argv;
  return (CmdResult){STATUS_EXIT_CMD, NULL, NULL};
}

CmdResult cmd_true(int argc, char** argv) {
  (void)argc;
  (void)argv;
  return ok_void();
}
CmdResult cmd_false(int argc, char** argv) {
  (void)argc;
  (void)argv;
  return err(NULL);
}

CmdResult cmd_whoami(int argc, char** argv) {
  (void)argc;
  (void)argv;

  char* user = getenv_dup("USER");
  if (!user) user = getenv_dup("USERNAME");
  if (!user) return err("unknown username");
  return ok(user);
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
  } cmds[] = {{"cat", cmd_cat},   {"clear", cmd_clear},  {"cls", cmd_clear},
              {"cwd", cmd_cwd},   {"date", cmd_date},    {"pwd", cmd_cwd},
              {"echo", cmd_echo}, {"exit", cmd_exit},    {"false", cmd_false},
              {"true", cmd_true}, {"whoami", cmd_whoami}};

  for (size_t i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++)
    cmd_cache_put(cache, cmds[i].name, cmds[i].f);

  return cache;
}
