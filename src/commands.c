// TODO: make aliases (including pwd and more)

#include <direct.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
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
  if (fd < 0) return err_from_errno();

  struct _stat st;
  if (_fstat(fd, &st) < 0) {
    _close(fd);
    return err_from_errno();
  }

  size_t size = st.st_size;
  char* buf = malloc(size + 1);  // +1 for '\0'
  if (!buf) {
    _close(fd);
    return err_from_errno();
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
    return err_from_errno();
  }

  buf[total] = '\0';  // null-terminate
  return ok(buf);
}

CmdResult cmd_clear(int argc, char** argv) {
  UNUSED_ARGS
  printf("\033[H\033[J");
  return ok_void();
}

CmdResult cmd_cwd(int argc, char** argv) {
  UNUSED_ARGS

  char* cwd = _getcwd(NULL, 0);
  if (!cwd) return err_from_errno();

  return ok(cwd);  // caller frees
}

CmdResult cmd_date(int argc, char** argv) {
  UNUSED_ARGS

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
  UNUSED_ARGS
  return (CmdResult){STATUS_EXIT_CMD, NULL, NULL};
}

CmdResult cmd_ls(int argc, char** argv) {
  printf("argc: %d\nargv[0]: %s\n", argc, argv[0]);
  const char* path = (argc > 1) ? argv[0] : ".";

  DIR* dir = opendir(path);
  if (!dir) return err_from_errno();

  struct dirent* entry;

  size_t cap = 1024;
  size_t len = 0;
  char* out = malloc(cap);
  if (!out) {
    closedir(dir);
    return oom();
  }

  out[0] = '\0';

  while ((entry = readdir(dir)) != NULL) {
    const char* name = entry->d_name;

    // skip "." and ".."
    if (name[0] == '.' &&
        (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')))
      continue;

    size_t n = strlen(name);

    // resize buffer if needed (+1 for newline, +1 for null)
    if (len + n + 2 > cap) {
      cap *= 2;
      char* tmp = realloc(out, cap);
      if (!tmp) {
        free(out);
        closedir(dir);
        return oom();
      }
      out = tmp;
    }

    memcpy(out + len, name, n);
    len += n;

    out[len++] = '\n';
    out[len] = '\0';
  }

  closedir(dir);

  if (len > 0 && out[len - 1] == '\n') out[len - 1] = '\0';

  return ok(out);
}

CmdResult cmd_true(int argc, char** argv) {
  UNUSED_ARGS
  return ok_void();
}
CmdResult cmd_false(int argc, char** argv) {
  UNUSED_ARGS
  return err(NULL);
}

CmdResult cmd_whoami(int argc, char** argv) {
  UNUSED_ARGS

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
  } cmds[] = {{"cat", cmd_cat},   {"clear", cmd_clear}, {"cls", cmd_clear},
              {"cwd", cmd_cwd},   {"date", cmd_date},   {"pwd", cmd_cwd},
              {"echo", cmd_echo}, {"exit", cmd_exit},   {"false", cmd_false},
              {"ls", cmd_ls},     {"true", cmd_true},   {"whoami", cmd_whoami}};

  for (size_t i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++)
    cmd_cache_put(cache, cmds[i].name, cmds[i].f);

  return cache;
}
