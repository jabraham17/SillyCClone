#ifndef _SEMA_AST_PT_CONVERSION_H_
#define _SEMA_AST_PT_CONVERSION_H_

#include "parser/parsetree.h"
#include "ast.h"

//convert parse tree to ast structure
module_t* pt_to_ast(pt_t* pt);

#endif
