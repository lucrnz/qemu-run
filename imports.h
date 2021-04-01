#ifndef QEMU_RUN_IMPORTS_H
#define QEMU_RUN_IMPORTS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

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
#else // !__NIX__
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

#ifdef DEBUG
#ifdef __GNUC__
#define dprint() printf("%s (%d) %s\n",__FILE__,__LINE__,__FUNCTION__);
#else
#define dprint() printf("%s (%d) %s\n",__FILE__,__LINE__);
#endif
#else // !DEBUG
#define dprint()
#endif // DEBUG

#define BUFF_AVG 128
#define BUFF_MAX BUFF_AVG*32

#ifndef stricmp //GCC is weird sometimes it doesn't includes this..
#include <strings.h>
#define stricmp(x,y) strcasecmp(x,y)
#endif

#include "config.h"
#include "liblucie/lucie_lib.h"

#define mzero_ca(a) memset(a, 0, sizeof(a));

#endif // QEMU_RUN_IMPORTS_H