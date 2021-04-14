#ifndef QEMU_RUN_IMPORTS_H
#define QEMU_RUN_IMPORTS_H

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#ifdef __GNUC__
#define DPRINT_S()                                                             \
    fprintf(stderr, "[DEBUG][%s %s:%d] Location reached.\n", __FUNCTION__,     \
            __FILE__, __LINE__)
#define DPRINT(...)                                                            \
    fprintf(stderr, "[DEBUG][%s %s:%d] ", __FUNCTION__, __FILE__, __LINE__);   \
    fprintf(stderr, __VA_ARGS__);                                              \
    fprintf(stderr, "\n")
#else
#define DPRINT_S()                                                             \
    fprintf(stderr, "[DEBUG][%s:%d]Location reached.\n", __FILE__, __LINE__);
#define DPRINT(...)                                                            \
    fprintf(stderr, "[DEBUG][%s:%d] ", __FILE__, __LINE__);                    \
    fprintf(stderr, __VA_ARGS__);                                              \
    fprintf(stderr, "\n")
#endif // __GNUC__
#else  // !DEBUG
#define NDEBUG
#define DPRINT_S()
#define DPRINT(...)
#endif // DEBUG

#include <assert.h>

#if defined(__unix__) || defined(__unix) || defined(unix)
#define __NIX__
#define PSEP ":"
#define DSEP "/"
#define PSEP_C ':'
#define DSEP_C '/'
#include <unistd.h>
#ifdef __linux__
#include <linux/limits.h>
#else
#include <limits.h>
#endif // __linux__
#else  // !__NIX__
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define __WINDOWS__
#define PSEP ";"
#define DSEP "\\"
#define PSEP_C ';'
#define DSEP_C '\\'
#include <io.h>
#include <limits.h>
#endif // __WINDOWS__
#endif // __NIX__
#include <sys/stat.h>
#include <sys/types.h>

#define BUFF_AVG 128
#define BUFF_MAX BUFF_AVG * 32

#ifndef stricmp // GCC is weird sometimes it doesn't includes this..
#include <strings.h>
#define stricmp(x, y) strcasecmp(x, y)
#endif

#include "config.h"
#include "liblucie/lucie_lib.h"

#define mzero_ca(a) memset(a, 0, sizeof(a));

#endif // QEMU_RUN_IMPORTS_H
