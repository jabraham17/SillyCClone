
#ifndef _COMMON_DEBUG_H_
    #define _COMMON_DEBUG_H_

// default debug mode is false
extern int _debug_mode;

    #if defined(DEBUG) && DEBUG == 1
        #define DEBUG_SECTION(X)                                               \
            do {                                                               \
                if(_debug_mode) {                                              \
                    X;                                                         \
                }                                                              \
            } while(0)
    #else
        #define DEBUG_SECTION(X)                                               \
            do {                                                               \
            } while(0)
    #endif

    #if defined(DEBUG) && DEBUG == 1
        #include <stdio.h>
        #include <string.h>

        #define GET_REL_FILE()                                                 \
            ((strstr(__FILE__, ROOT_PROJECT_DIRECTORY)) +                      \
             strlen(ROOT_PROJECT_DIRECTORY))

        // MUST be defined in one line so __LINE__ works right
        // clang-format off
        #define DEBUG_LOG_FP(fp, msg, ...) do { if(_debug_mode) { fprintf(fp, "%s:%s:%d: "msg, GET_REL_FILE(), __func__, __LINE__ __VA_OPT__(,) __VA_ARGS__); } } while(0)
    // clang-format on
    #else
        #define DEBUG_LOG_FP(fp, msg, ...)                                     \
            do {                                                               \
            } while(0)
    #endif
    #define DEBUG_LOG(msg, ...) DEBUG_LOG_FP(stderr, msg __VA_OPT__(,) __VA_ARGS__)

#endif

//__func__ ":"__line__
