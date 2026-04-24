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

static char *expand_path(const char *path) {
    if (!path || path[0] != '~')
        return xstrdup(path);

    const char *home = getenv("USERPROFILE");
    if (!home) {
        const char *drive = getenv("HOMEDRIVE");
        const char *pathp = getenv("HOMEPATH");
        if (drive && pathp) {
            size_t len = strlen(drive) + strlen(pathp) + 1;
            char *tmp = malloc(len);
            if (!tmp)
                return NULL;
            snprintf(tmp, len, "%s%s", drive, pathp);
            home = tmp;
            return tmp; // already allocated
        }
        return NULL;
    }

    // "~" or "~/something"
    size_t home_len = strlen(home);
    size_t rest_len = strlen(path + 1); // skip '~'

    char *out = malloc(home_len + rest_len + 1);
    if (!out)
        return NULL;

    memcpy(out, home, home_len);
    memcpy(out + home_len, path + 1, rest_len + 1);

    return out;
}

CmdResult cmd_cat(int argc, char **argv) {
    if (argc < 2)
        return err("no file specified");

    const char *filename = argv[1];

    int fd = _open(filename, _O_RDONLY | _O_BINARY);
    if (fd < 0)
        return err_from_errno();

    struct _stat st;
    if (_fstat(fd, &st) < 0) {
        _close(fd);
        return err_from_errno();
    }

    size_t size = st.st_size;
    char *buf = malloc(size + 1); // +1 for '\0'
    if (!buf) {
        _close(fd);
        return err_from_errno();
    }

    size_t total = 0;
    ssize_t n;
    while (total < size &&
           (n = _read(fd, buf + total,
                      CAT_BUF_SIZE < size - total ? CAT_BUF_SIZE
                                                  : size - total)) > 0)
        total += n;

    _close(fd);

    if (total != size) { // read error
        free(buf);
        return err_from_errno();
    }

    buf[total] = '\0';
    return ok(buf);
}

CmdResult cmd_cd(int argc, char **argv) {
    const char *raw = (argc < 2) ? NULL : argv[1];

    if (!raw || raw[0] == '\0') {
        raw = getenv("HOME");
        if (!raw)
            raw = getenv("USERPROFILE");
        if (!raw)
            return err("HOME not set");
    }

    char *target = expand_path(raw);
    if (!target)
        return err("path expansion failed");

    if (target[0] == '\0') {
        free(target);
        return err("empty path");
    }

    if (chdir(target) != 0) {
        CmdResult r = err_from_errno();
        free(target);
        return r;
    }

    free(target);
    return ok_void();
}

CmdResult cmd_clear(int argc, char **argv) {
    UNUSED_ARGS
    printf("\033[3J\033[2J\033[H");
    fflush(stdout);
    return ok_void();
}

CmdResult cmd_cwd(int argc, char **argv) {
    UNUSED_ARGS

    char *cwd = _getcwd(NULL, 0);
    if (!cwd)
        return err_from_errno();

    return ok(cwd); // caller frees
}

CmdResult cmd_date(int argc, char **argv) {
    UNUSED_ARGS

    char *out = format_time("%a %b %d %H:%M:%S %Z %Y");
    if (!out)
        return err("time format failed");
    return ok(out);
}

CmdResult cmd_echo(int argc, char **argv) {
    size_t len = 1; // '\0'

    for (int i = 1; i < argc; i++) {
        len += strlen(argv[i]);
        if (i < argc - 1)
            len += 1;
    }

    char *out = malloc(len);
    if (!out)
        return oom();

    char *p = out;

    for (int i = 1; i < argc; i++) {
        size_t l = strlen(argv[i]);
        memcpy(p, argv[i], l);
        p += l;

        if (i < argc - 1)
            *p++ = ' ';
    }

    *p = '\0';
    return ok(out);
}

CmdResult cmd_exit(int argc, char **argv) {
    UNUSED_ARGS
    return (CmdResult){STATUS_EXIT_CMD, NULL, NULL};
}

CmdResult cmd_ls(int argc, char **argv) {
    const char *raw = (argc > 1) ? argv[1] : ".";
    char *path = expand_path(raw);
    if (!path)
        return err("failed to expand path");

    DIR *dir = opendir(path);
    free(path);
    if (!dir)
        return err_from_errno();

    size_t cap = 1024;
    size_t len = 0;

    char *out = malloc(cap);
    if (!out) {
        closedir(dir);
        return oom();
    }

    struct dirent *e;

    while ((e = readdir(dir))) {
        const char *name = e->d_name;

        if (name[0] == '.' &&
            (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')))
            continue;

        size_t n = strlen(name);

        if (len + n + 2 > cap) {
            while (len + n + 2 > cap)
                cap *= 2;

            char *tmp = realloc(out, cap);
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
    }

    closedir(dir);

    if (len > 0)
        len--;
    out[len] = '\0';

    return ok(out);
}

CmdResult cmd_true(int argc, char **argv) {
    UNUSED_ARGS
    return ok_void();
}
CmdResult cmd_false(int argc, char **argv) {
    UNUSED_ARGS
    return err(NULL);
}

CmdResult cmd_whoami(int argc, char **argv) {
    UNUSED_ARGS

    char *user = getenv_dup("USER");
    if (!user)
        user = getenv_dup("USERNAME");
    if (!user)
        return err("unknown username");
    return ok(user);
}

/**
 * @brief new commands cache/registry
 *
 * @return freshly made cmd cache
 */
CmdCache *new_cc(void) {
    CmdCache *cache = cmd_cache_init(INIT_CC_CAPACITY);

    static struct {
        const char *name;
        CmdFn f;
    } cmds[] = {{"cat", cmd_cat},      {"cd", cmd_cd},     {"clear", cmd_clear},
                {"cls", cmd_clear},    {"cwd", cmd_cwd},   {"date", cmd_date},
                {"pwd", cmd_cwd},      {"echo", cmd_echo}, {"exit", cmd_exit},
                {"false", cmd_false},  {"ls", cmd_ls},     {"true", cmd_true},
                {"whoami", cmd_whoami}};

    for (size_t i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++)
        cmd_cache_put(cache, cmds[i].name, cmds[i].f);

    return cache;
}
