#ifndef _TOOLS_INTERP_IR_INTERP_H_
#define _TOOLS_INTERP_IR_INTERP_H_
#include "ir/ir.h"
#include <stdio.h>

void execute_ir(FILE* fp, ir_function_t* function);

#endif
