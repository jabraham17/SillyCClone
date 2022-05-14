#ifndef _TOOLS_HELPER_ARGS_FILE_HELPER_H_
#define _TOOLS_HELPER_ARGS_FILE_HELPER_H_

#define OPEN_FILE_OR_FILENO(_handle, _file_name, _fileno, _options)            \
    do {                                                                       \
        _handle = NULL;                                                        \
        if(_file_name != NULL) {                                               \
            _handle = fopen(_file_name, _options);                             \
        } else {                                                               \
            int _fileno_dup = dup(fileno(_fileno));                            \
            _handle = fdopen(_fileno_dup, _options);                           \
        }                                                                      \
        if(!_handle) {                                                         \
            fprintf(stderr, "Invalid file\n");                                 \
            exit(1);                                                           \
        }                                                                      \
    } while(0)

#define OPEN_FILE_OR_STDIN(_handle, _file_name)                                \
    OPEN_FILE_OR_FILENO(_handle, _file_name, stdin, "r")

#define OPEN_FILE_OR_STDOUT(_handle, _file_name)                               \
    OPEN_FILE_OR_FILENO(_handle, _file_name, stdout, "w")

#endif
