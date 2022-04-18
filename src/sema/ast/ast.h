#ifndef _SEMA_AST_AST_H_
#define _SEMA_AST_AST_H_

#define _PREDECLARE_STRUCT_TYPE(_name) typedef struct _name _name##_t;
_PREDECLARE_STRUCT_TYPE(function)
_PREDECLARE_STRUCT_TYPE(function_list)
_PREDECLARE_STRUCT_TYPE(scope)
_PREDECLARE_STRUCT_TYPE(ast_stmt)
_PREDECLARE_STRUCT_TYPE(ast_stmt_list)
_PREDECLARE_STRUCT_TYPE(variable)
_PREDECLARE_STRUCT_TYPE(variable_list)
#undef _PREDECLARE_STRUCT_TYPE

#define _MAKE_LIST_FOR_TYPE(_name)                                             \
    struct _name##_list {                                                      \
        _name##_t* _name;                                                      \
        _name##_list_t* next;                                                  \
        _name##_list_t* prev;                                                  \
    };

struct function {
    char* name;
    variable_list_t* params;
    scope_t* scope;
};
_MAKE_LIST_FOR_TYPE(function)

struct scope {
    scope_t* parent_scope;
    function_t* function;
    ast_stmt_list_t* statements;
    variable_list_t* variables;
};

#define _AST_TYPES(V)                                                          \
    V(NOP)                                                                     \
    V(NUMBER)                                                                  \
    V(VARIABLE)                                                                \
    V(EQUALS)                                                                  \
    V(DEQUALS)                                                                 \
    V(PLUS)                                                                    \
    V(MINUS)                                                                   \
    V(IF)                                                                      \
    V(WHILE)                                                                   \
    V(RETURN)                                                                  \
    V(CALL)                                                                    \
    V(SCOPE)

typedef enum {
#define AST_ENUM(_NAME) ast_##_NAME,
    _AST_TYPES(AST_ENUM)
#undef AST_ENUM
} ast_stmt_type_t;
char* getASTTypeString(ast_stmt_type_t t);


struct ast_stmt {
    ast_stmt_type_t type;
    union {
        variable_t* variable;
        int num;
        scope_t* scope;
    } data;
    ast_stmt_t* left;
    ast_stmt_t* right;
};
_MAKE_LIST_FOR_TYPE(ast_stmt)

struct variable {
    char* name;
    scope_t* parent_scope;
};
_MAKE_LIST_FOR_TYPE(variable)

#endif
