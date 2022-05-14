#ifndef _TOOLS_HELPER_ARGS_STRING_LIST_H_
#define _TOOLS_HELPER_ARGS_STRING_LIST_H_

// makes a linked list of strings that can then be converted quickly to an array
// prev of head holds the tail pointer

struct sl_ll_t {
    char* str;
    int idx;
    struct sl_ll_t* next;
    struct sl_ll_t* prev;
};

#define SL_INIT(_head) struct sl_ll_t* _head = NULL;

#define _SL_ALLOC(_var, _str, _idx)                                            \
    do {                                                                       \
        (_var) = malloc(sizeof(*(_var)));                                      \
        (_var)->str = (_str);                                                  \
        (_var)->idx = (_idx);                                                  \
        (_var)->next = NULL;                                                   \
        (_var)->prev = NULL;                                                   \
    } while(0)

// maintain the heads prev as a tail pointer
#define SL_ADD(_head, _str)                                                    \
    do {                                                                       \
        if(!(_head)) {                                                         \
            _SL_ALLOC((_head), (_str), 0);                                     \
            (_head)->prev = (_head);                                           \
        } else {                                                               \
            _SL_ALLOC((_head)->prev->next, (_str), (_head)->prev->idx + 1);    \
            (_head)->prev->next->prev = (_head)->prev;                         \
            (_head)->prev->next = (_head)->prev->next;                         \
            (_head)->prev = (_head)->prev->next;                               \
        }                                                                      \
    } while(0)

// convert the ll to an array and free the ll cleanly
// + 2, since the idx is 0 based and we need an extra for the NULL
#define SL_ARRAY(_head, _array)                                                \
    char** _array = NULL;                                                      \
    do {                                                                       \
        if((_head)) {                                                          \
            (_array) = malloc(sizeof(*(_array)) * (_head)->prev->idx + 2);     \
            (_array)[(_head)->prev->idx + 1] = NULL;                           \
            while((_head)) {                                                   \
                (_array)[_head->idx] = (_head)->str;                           \
                (_head) = (_head)->next;                                       \
                /*if(_head) free((_head)->prev);*/                             \
            }                                                                  \
        }                                                                      \
    } while(0)

#endif
