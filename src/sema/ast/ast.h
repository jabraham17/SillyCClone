#ifndef _SEMA_AST_AST_H_
#define _SEMA_AST_AST_H_

#define _PREDECLARE_STRUCT_TYPE(_name) typedef struct _name _name##_t;
_PREDECLARE_STRUCT_TYPE(function)
_PREDECLARE_STRUCT_TYPE(function_list)
_PREDECLARE_STRUCT_TYPE(ast_stmt)
_PREDECLARE_STRUCT_TYPE(symbol)
_PREDECLARE_STRUCT_TYPE(symbol_list)
#undef _PREDECLARE_STRUCT_TYPE

#define _MAKE_LIST_FOR_TYPE(_name)                                             \
    struct _name##_list {                                                      \
        _name##_t* _name;                                                      \
        _name##_list_t* next;                                                  \
        _name##_list_t* prev;                                                  \
    };

struct function {
    symbol_t* symbol;
    symbol_list_t* params;
    symbol_list_t* locals;
    ast_stmt_t* stmts;
};
_MAKE_LIST_FOR_TYPE(function)

#define _AST_TYPES(V)                                                          \
    V(NOP)                                                                     \
    V(NUMBER)                                                                  \
    V(SYMBOL)                                                                  \
    V(EQUALS)                                                                  \
    V(DEQUALS)                                                                 \
    V(PLUS)                                                                    \
    V(MINUS)                                                                   \
    V(IF)                                                                      \
    V(WHILE)                                                                   \
    V(RETURN)                                                                  \
    V(CALL)                                                                    \
    V(PRINT)

typedef enum {
#define AST_ENUM(_NAME) ast_##_NAME,
    _AST_TYPES(AST_ENUM)
#undef AST_ENUM
} ast_stmt_type_t;
char* getASTTypeString(ast_stmt_type_t t);

struct ast_stmt {
    ast_stmt_type_t type;
    union {
        symbol_t* symbol;
        int num;
    } data;
    ast_stmt_t* left;
    ast_stmt_t* right;
    ast_stmt_t* next;
};

typedef enum {
    st_ERROR = 0,
    st_FUNCTION = 1,
    st_PARAM = 2,
    st_LOCAL = 4,
} symbol_flags_t;
struct symbol {
    char* name;
    symbol_flags_t flags;
};
_MAKE_LIST_FOR_TYPE(symbol)

typedef struct symbol_table_entry symbol_table_entry_t;
struct symbol_table_entry {
    function_t* function_owner;
    symbol_t* symbol;
    symbol_table_entry_t* next;
    symbol_table_entry_t* prev;
};

typedef struct module module_t;
struct module {
    function_list_t* functions;
    symbol_table_entry_t* symbol_table;
};
symbol_table_entry_t*
hasSymbol(module_t* module, function_t* function, char* name);
symbol_table_entry_t* insertSymbol(
    module_t* module, function_t* function, char* name, symbol_flags_t flags);
void propogateSymbol(function_t* function, symbol_table_entry_t* ste);

#endif
