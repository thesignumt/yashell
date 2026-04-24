#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define UNUSED_ARGS                                                            \
    (void)argc;                                                                \
    (void)argv;

#define _DEBUG_ARGS(argc, argv)                                                \
    do {                                                                       \
        fprintf(stderr, "[DEBUG] %s:%d %s\n", __FILE__, __LINE__, __func__);   \
        fprintf(stderr, "argc: %d\n", (argc));                                 \
        for (int _i = 0; _i < (argc); ++_i)                                    \
            fprintf(stderr, "argv[%d]: %s\n", _i, (argv)[_i]);                 \
    } while (0)

#define INIT_CC_CAPACITY 64
#define DATE_BUFFER 100
#define CAT_BUF_SIZE 65536

typedef struct CmdCache CmdCache;

typedef enum {
    STATUS_UNSET = -1,
    STATUS_SUCCESS,
    STATUS_ERROR,
    STATUS_EXIT_CMD,
    STATUS_CMD_NOT_FOUND
    // TODO: STATUS_CTRL_C = 4
} CmdStatus;

typedef struct {
    CmdStatus status; // 0 = success, non-zero = error code
    char *output;     // optional string output from the command
    void *data;       // optional pointer to extra data (if needed)
} CmdResult;

typedef CmdResult (*CmdFn)(int argc, char *argv[]);

////////////////////////////////////////////////////////////

static inline char *xstrdup(const char *s) {
    if (!s)
        return NULL;

    char *p = strdup(s);
    if (!p)
        return NULL; // let caller convert to oom()
    return p;
}

static inline CmdResult cmd_result(CmdStatus status, char *output, void *data) {
    return (CmdResult){status, output, data};
}

static inline CmdResult ok(char *output) {
    return cmd_result(STATUS_SUCCESS, output, NULL);
}

static inline CmdResult ok_void(void) {
    return cmd_result(STATUS_SUCCESS, NULL, NULL);
}

static inline CmdResult err(const char *msg) {
    if (!msg)
        return cmd_result(STATUS_ERROR, NULL, NULL);

    char *copy = xstrdup(msg);
    return cmd_result(STATUS_ERROR, copy, NULL);
}

static inline CmdResult err_from_errno(void) { return err(strerror(errno)); }

static inline CmdResult oom(void) { return err("out of memory"); }

static inline char *getenv_dup(const char *name) {
    const char *val = getenv(name);
    return val ? xstrdup(val) : NULL;
}

static inline char *format_time(const char *fmt) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (!t)
        return NULL;

    char buf[DATE_BUFFER];
    if (!strftime(buf, sizeof(buf), fmt, t))
        return NULL;

    return xstrdup(buf);
}

////////////////////////////////////////////////////////////

CmdCache *new_cc(void);

#endif // COMMANDS_H_
