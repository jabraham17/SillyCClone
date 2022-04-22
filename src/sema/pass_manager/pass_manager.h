#ifndef _SEMA_PASS_MANAGER_PASS_MANAGER_H_
#define _SEMA_PASS_MANAGER_PASS_MANAGER_H_

#include "sema/ast/ast.h"

// function to transform and optional module arg
typedef void (*func_pass_t)(function_t**, module_t*);
typedef struct pass pass_t;
struct pass {
    func_pass_t func_pass;

    pass_t* next;
};

pass_t* buildPass(func_pass_t func_pass);
// last pass MUST be NULL
pass_t* chainPass(pass_t* p1, ...);
void runPass(module_t* module, pass_t* pass);

#define DEFINE_PASS(funcname)                                                  \
    void PASS_##funcname(                                                      \
        __attribute((__unused__)) function_t** func_ptr,                       \
        __attribute((__unused__)) module_t* module)

#define PASS_TUPLE(funcname) DEFINE_PASS(funcname);
#include "passes.inc"
#undef PASS_TUPLE

#define COMPOUND_PASS_TUPLE(funcname, ...)                                     \
    DEFINE_PASS(funcname);                                                     \
    pass_t* getPass_##funcname();
#include "passes.inc"
#undef COMPOUND_PASS_TUPLE

#endif
