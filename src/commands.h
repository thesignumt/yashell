#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <stddef.h>

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

CmdCache *new_cc(void);

#endif // COMMANDS_H_
