#ifndef _SEMA_AST_AST_H_
#define _SEMA_AST_AST_H_

#define _PREDECLARE_STRUCT_TYPE(_name) typedef struct _name _name##_t;
_PREDECLARE_STRUCT_TYPE(function)
_PREDECLARE_STRUCT_TYPE(function_list)
_PREDECLARE_STRUCT_TYPE(scope)
_PREDECLARE_STRUCT_TYPE(assign_stmt)
_PREDECLARE_STRUCT_TYPE(if_stmt)
_PREDECLARE_STRUCT_TYPE(while_stmt)
_PREDECLARE_STRUCT_TYPE(return_stmt)
_PREDECLARE_STRUCT_TYPE(call_stmt)
_PREDECLARE_STRUCT_TYPE(expression)
_PREDECLARE_STRUCT_TYPE(statement)
_PREDECLARE_STRUCT_TYPE(statement_list)
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
    statement_list_t* statements;
    variable_list_t* variables;
};

typedef enum {
    st_ASSIGN_STMT,
    st_IF_STMT,
    st_WHILE_STMT,
    st_RETURN_STMT,
    st_CALL,
} statement_type_t;

struct assign_stmt {};
struct if_stmt {};
struct while_stmt {};
struct return_stmt {};
struct call_stmt {};

typedef enum {
    op_NOP,
    op_ op_PLUS,
    op_MINUS,
    op_DEQUALS,
} operator_type_t;

struct expression {
    operator_type_t op;
    expression_t* expr1;
    expression_t* expr2;
};

struct statement {
    statement_type_t type;
    // type specific data
    union {
        assign_stmt_t* assign_stmt;
        if_stmt_t* if_stmt;
        while_stmt_t* while_stmt;
        return_stmt_t* return_stmt;
        call_stmt_t* call_stmt;
    };
    scope_t* parent_scope;
};
_MAKE_LIST_FOR_TYPE(statement)

struct variable {
    char* name;
    scope_t* parent_scope;
};
_MAKE_LIST_FOR_TYPE(variable)

#endif
