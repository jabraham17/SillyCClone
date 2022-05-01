
#ifndef _COMMON_DEBUG_H_
#define _COMMON_DEBUG_H_

// default debug mode is false
extern int _debug_mode;

#if defined(DEBUG) && DEBUG == 1
    #define DEBUG_SECTION(X)                                                   \
        do {                                                                   \
            if(_debug_mode) {                                                  \
                X;                                                             \
            }                                                                  \
        } while(0)
#else
    #define DEBUG_SECTION(X)                                                   \
        do {                                                                   \
        } while(0)
#endif

#if defined(DEBUG) && DEBUG == 1
    #include <stdio.h>
    // MUST be defined in one line so __LINE__ works right
    // clang-format off
    #define DEBUG_LOG_FP(msg, fp) do { if(_debug_mode) fprintf(fp, "%s:%s:%d: %s", __FILE__, __func__, __LINE__, msg); } while(0)
    // clang-format on
#else
    #define DEBUG_LOG_FP(msg, fp)                                              \
        do {                                                                   \
        } while(0)
#endif
#define DEBUG_LOG(msg) DEBUG_LOG_FP(msg, stderr)

#endif

//__func__ ":"__line__
