
#include "sema/pass_manager/pass_manager.h"
#include <string.h>

// DEFINE_PASS(silly_pass) {
//     function_t* func = *func_ptr;
//     LOG_PASS(silly_pass, *func_ptr);

//     strcpy(func->name, "hewo");
// }
// DEFINE_PASS(silly_pass2) {
//     LOG_PASS(silly_pass2, *func_ptr);
//     function_t* func = *func_ptr;

//     func->params = NULL;
// }
// DEFINE_PASS(silly_pass3) {
//     function_t* func = *func_ptr;
//     func->name[0] = 'F';
// }
