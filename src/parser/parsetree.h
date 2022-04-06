#ifndef _PARSER_PARSETREE_H_
#define _PARSER_PARSETREE_H_

typedef struct id_list id_list_t;
id_list_t* getIDNode(char* id, id_list_t* oldList);

#define _PT_TYPES(V)                                                          \
    V(NONE)                                                                    \
    V(NUM)                                                                     \
    V(STR)                                                                     \
    V(STMT)                                                                    \
    /* operators */                                                            \
    V(PLUS)                                                                    \
    V(MINUS)                                                                   \
    V(DEQUALS)                                                                 \
    V(EQUALS)                                                                  \
    /* control flow */                                                         \
    V(IF)                                                                      \
    V(WHILE)                                                                   \
    V(RETURN)                                                                  \
    /* functions */                                                            \
    V(FUNCTION_DEF)                                                            \
    V(CALL)                                                                    \
    V(SCOPE)                                                                   \
    /* semantic char lists */                                                  \
    V(PARAM_DEF)                                                               \
    V(VAR_DEF)                                                                 \
    V(PARAM_LIST)

typedef enum {
#define PT_ENUM(_NAME) pt_##_NAME,
    _PT_TYPES(PT_ENUM)
#undef PT_ENUM

} pt_type_t;

char* getPTTypeString(pt_type_t t);

#define _PT_MAX_CHILDREN 8
typedef struct pt pt_t;
struct pt {
    pt_type_t type;
    union {
        int num;
        char* str;
        char** ids; // lpt elm MUST be NULL
    } data;
    pt_t* children[_PT_MAX_CHILDREN + 1]; // lpt elm MUST be NULL
};
pt_t* allocatePT();
pt_t* allocatePT_TYPE(pt_type_t t);
pt_t* allocatePT_NUM(int num);
pt_t* allocatePT_STR(char* str);
pt_t* allocatePT_ID_LIST(pt_type_t t, id_list_t* ids);
void addChild(pt_t* parent, pt_t* child);

// #define _PT_TYPES(V)                                                          \
//     V(expr)                                                                    \
//     V(scope)                                                                   \
//     V(statement)                                                               \
//     V(statement_list)                                                          \
//     V(symbol)                                                                  \
//     V(symbol_list)                                                             \
//     V(function)                                                                \
//     V(function_list)

// #define _predeclare_structs(_basename)                                         \
//     struct _basename;                                                          \
//     typedef struct _basename _basename##_t;
// _PT_TYPES(_predeclare_structs)
// #undef _predeclare_structs

// typedef enum {
//     st_ASSIGN_STMT,
//     st_FUNCTION_CALL,
//     st_RETURN_STMT,
//     st_IF_STMT,
//     st_WHILE_STMT
// } statement_type_t;

// typedef enum { op_NOP, p_PLUS, op_MINUS, op_DEQUALS } operator_type_t;
// typedef enum { ss_FUNCTION, ss_VARIABLE, ss_CONSTANT } symbol_type_t;

// struct expr {
//     operator_type_t op;
//     symbol_t* e1;
//     symbol_t* e2;
// };

// expr_t* createExpression(operator_type_t op, symbol_t* e1, symbol_t* e2);

// struct scope {
//     symbol_list_t* syms;
//     statement_list_t* stmts;
// };

// struct symbol {
//     symbol_type_t tt;
//     union {
//         function_t* func;
//         char* var;
//         int constant;
//     };
// };
// struct symbol_list {
//     symbol_t sym;
//     struct symbol_list* next;
// };

// symbol_t* getSymbolForVariable(char* var);
// symbol_t* getSymbolForConstant(int constant);
// symbol_t* getSymbolForFunction(function_t* func);
// symbol_t* getSymbolForFunctionName(char* name);

// symbol_t* createSymbolForVariable(char* var);
// symbol_t* createSymbolForConstant(int constant);
// symbol_t* createSymbolForFunction(function_t* func);

// struct statement {
//     statement_type_t tt;
//     symbol_t* sym;
//     expr_t* expr;
//     scope_t* scope;
//     symbol_list_t* sym_list
// };
// struct statement_list {
//     statement_t stmt;
//     statement_list_t* next;
// };

// statement_t* createAssignStatement(symbol_t* sym, expr_t* expr);
// statement_t* createFunctionCallStatement(symbol_t* sym, symbol_list_t*
// sym_list); statement_t* createReturnStatement(expr_t* expr); statement_t*
// createIfStatement(expr_t* expr, scope_t* scope); statement_t*
// createWhileStatement(expr_t* expr, scope_t* scope);

// struct function {
//     char* name;
//     symbol_list_t* params;
//     scope_t* scope;
// };
// struct function_list {
//     function_t func;
//     function_list_t* next;
// };

// #undef _PT_TYPES

#endif
