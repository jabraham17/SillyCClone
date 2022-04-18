#include "ast.h"

char* getASTTypeString(ast_stmt_type_t t) {
    switch(t) {
#define AST_CASE(_NAME)                                                        \
    case ast_##_NAME:                                                          \
        return #_NAME;
        _AST_TYPES(AST_CASE)
#undef AST_CASE
    }
}
