#ifndef P101_TOOL_PLAYGROUND_CONSTANTS_H
#define P101_TOOL_PLAYGROUND_CONSTANTS_H

#define DEFAULT_SCENARIO "tour"
#define DEFAULT_OUTPUT_PATH "/tmp/p101-tool-playground-output.txt"

enum
{
    MSG_LEN          = 256,
    DEFAULT_BYTES    = 64,
    DEFAULT_REPEATS  = 2,
    MAX_BYTES        = 4096,
    MAX_REPEATS      = 32,
    READ_BUF_LEN     = 128,
    STRAY_CLOSE_FD   = 999,
    REPORT_FILE_MODE = 0644,
    EXEC_FAILURE     = 127
};

#endif    // P101_TOOL_PLAYGROUND_CONSTANTS_H
