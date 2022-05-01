#ifndef _PARSER_AST_H_
#define _PARSER_AST_H_

typedef struct id_list {
    char* id;
    struct id_list* next;
} id_list_t;

struct symbol;
struct scope;

typedef enum {
    st_ASSIGN_STMT,
    st_FUNCTION_CALL,
    st_RETURN_STMT,
    st_IF_STMT,
    st_WHILE_STMT
} statement_type_t;
typedef enum { op_PLUS, op_MINUS, op_DEQUALS } operator_type_t;
typedef struct expr {
    struct symbol* e1;
    struct symbol* e2;
    operator_type_t op;
} expr_t;

typedef struct statement {
    statement_type_t tt;
    union {
        struct {
            struct symbol* lhs;
            expr_t* expr;
        } assign_stmt;
        struct {
            struct symbol* function;
        } function_call;
        struct {
            struct symbol* retval;
        } return_stmt;
        struct {
            expr_t* expr;
            struct scope* scope;
        } if_stmt;
        struct {
            expr_t* expr;
            struct scope* scope;
        } while_stmt;
        struct scope* scope;
    };
    struct statement* next;
} statement_t;

typedef struct {
    id_list_t* vars;
    statement_t* stmt_list;
} scope_t;

typedef struct function {
    char* name;
    id_list_t* params;
    scope_t* scope;
    struct function* next;
} function_t;

typedef enum { ss_FUNCTION, ss_VARIABLE, ss_CONSTANT } symbol_type_t;
typedef struct symbol {
    symbol_type_t tt;
    union {
        function_t* func;
        char* var;
        int constant;
    };
    struct symbol* next;
} symbol_t;

typedef struct {
    function_t* functionList;
    symbol_t* symbolTable;
} program_t;

#endif
