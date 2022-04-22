#ifndef _SEMA_AST_AST_H_
#define _SEMA_AST_AST_H_

#define _PREDECLARE_STRUCT_TYPE(_name) typedef struct _name _name##_t;
_PREDECLARE_STRUCT_TYPE(function)
_PREDECLARE_STRUCT_TYPE(function_list)
_PREDECLARE_STRUCT_TYPE(ast_stmt)
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
    variable_list_t* locals;
    ast_stmt_t* stmts;
};
_MAKE_LIST_FOR_TYPE(function)


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
    V(CALL)                                                                    

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
    } data;
    ast_stmt_t* left;
    ast_stmt_t* right;
    ast_stmt_t* next;
};

struct variable {
    char* name;
};
_MAKE_LIST_FOR_TYPE(variable)



typedef enum {
    isFunction = 1,
    isParam = 2,
    isLocal = 4,
} symbol_table_entry_flags_t;

typedef struct symbol_table_entry symbol_table_entry_t;
struct symbol_table_entry {
    function_t* function_owner;
    symbol_table_entry_flags_t flags;
    variable_t* name;
    symbol_table_entry_t* next;
    symbol_table_entry_t* prev;
};

typedef struct module module_t;
struct module {
    function_list_t* functions;
    symbol_table_entry_t* symbol_table;
};

#endif
