#include "ast.h"
#include "common/utlist.h"
#include <stdlib.h>
#include <string.h>

char* getASTTypeString(ast_stmt_type_t t) {
    switch(t) {
#define AST_CASE(_NAME)                                                        \
    case ast_##_NAME:                                                          \
        return #_NAME;
        _AST_TYPES(AST_CASE)
#undef AST_CASE
    }
    return "";
}

symbol_table_entry_t*
hasSymbol(module_t* module, function_t* function, char* name) {
    symbol_table_entry_t* ste = NULL;
    DL_FOREACH(module->symbol_table, ste) {
        if((ste->function_owner == function || ste->function_owner == NULL) &&
           strcmp(ste->symbol->name, name) == 0) {
            return ste;
        }
    }
    return NULL;
}

#include "common/debug.h"

symbol_table_entry_t* insertSymbol(
    module_t* module, function_t* function, char* name, symbol_flags_t flags) {

    symbol_table_entry_t* ste = malloc(sizeof(*ste));
    ste->function_owner = function;

    symbol_t* sym = malloc(sizeof(*sym));
    sym->name = strdup(name);
    sym->flags = flags;
    ste->symbol = sym;

    ste->next = NULL;
    ste->prev = NULL;

    DL_APPEND(module->symbol_table, ste);

    return ste;
}

void propogateSymbolInStatement(ast_stmt_t* stmt, symbol_table_entry_t* ste);
void propogateSymbolInStatements(ast_stmt_t* stmts, symbol_table_entry_t* ste);

void propogateSymbol(function_t* function, symbol_table_entry_t* ste) {
    // replace all params with symbol

    symbol_list_t* elm_list = NULL;
    DL_FOREACH(function->params, elm_list) {
        if(strcmp(elm_list->symbol->name, ste->symbol->name) == 0) {
            elm_list->symbol = ste->symbol;
        }
    }

    // replace all locals with symbol

    DL_FOREACH(function->locals, elm_list) {
        if(strcmp(elm_list->symbol->name, ste->symbol->name) == 0) {
            elm_list->symbol = ste->symbol;
        }
    }

    propogateSymbolInStatements(function->stmts, ste);
}

void propogateSymbolInStatement(ast_stmt_t* stmt, symbol_table_entry_t* ste) {
    if(stmt == NULL)
        return;
    // if the names match, set the symbol
    if(stmt->type == ast_SYMBOL &&
       strcmp(stmt->data.symbol->name, ste->symbol->name) == 0) {
        stmt->data.symbol = ste->symbol;
    }

    if(stmt->left != NULL) {
        propogateSymbolInStatements(stmt->left, ste);
    }
    if(stmt->right != NULL) {
        propogateSymbolInStatements(stmt->right, ste);
    }
}

// returns id of list head
void propogateSymbolInStatements(ast_stmt_t* stmts, symbol_table_entry_t* ste) {
    ast_stmt_t* temp = NULL;
    LL_FOREACH(stmts, temp) { propogateSymbolInStatement(temp, ste); }
}
