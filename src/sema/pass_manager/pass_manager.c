#include "pass_manager.h"
#include "common/macro_map.h"
#include <stdarg.h>
#include <stdlib.h>

pass_t* buildPass(func_pass_t func_pass) {
    pass_t* p = malloc(sizeof(*p));
    p->func_pass = func_pass;
    p->next = NULL;
    return p;
}

#define MOVE_LL_TO_LAST_ELM(ll)                                                \
    while((ll) != NULL && (ll)->next != NULL)                                  \
        (ll) = (ll)->next;

// last pass MUST be NULL
pass_t* chainPass(pass_t* p1, ...) {
    va_list args;
    va_start(args, p1);
    pass_t* lastElm = p1;
    while(1) {
        pass_t* newP = va_arg(args, pass_t*);
        if(newP == NULL)
            break;
        MOVE_LL_TO_LAST_ELM(lastElm);
        lastElm->next = newP;
    }
    va_end(args);
    return p1;
}
void runPass(module_t* module, pass_t* pass) {
    function_list_t* funcs = module->functions;
    while(funcs != NULL) {
        pass->func_pass(&(funcs->function), module);
        funcs = funcs->next;
    }
    if(pass->next)
        runPass(module, pass->next);
}

#define RUN_PASS(funcname) buildPass(PASS_##funcname)->func_pass(func_ptr, module);
#define COMPOUND_PASS_TUPLE(funcname, ...)                                     \
    DEFINE_PASS(funcname) { EVAL(MAP(RUN_PASS, __VA_ARGS__)) }
#include "passes.inc"
#undef COMPOUND_PASS_TUPLE
#undef RUN_PASS
