#include "common/utlist.h"
#include "sema/pass_manager/pass_manager.h"

// insert symbol for function name
DEFINE_PASS(build_symbols_function) {
    function_t* func = *func_ptr;
    LOG_PASS(build_symbols_function, func);

    symbol_table_entry_t* ste = hasSymbol(module, NULL, func->symbol->name);
    if(ste == NULL) {
        ste = insertSymbol(module, NULL, func->symbol->name, st_FUNCTION);
        func->symbol = ste->symbol;
    } else {
        ste->symbol->flags = st_ERROR;
    }
}

DEFINE_PASS(build_symbols_params) {
    function_t* func = *func_ptr;
    LOG_PASS(build_symbols_params, func);

    // we can safely assume no symbols exist for params, as this is the first
    // pass
    symbol_list_t* sym_list = NULL;
    DL_FOREACH(func->params, sym_list) {

        symbol_table_entry_t* ste =
            hasSymbol(module, func, sym_list->symbol->name);
        if(ste == NULL) {
            ste = insertSymbol(module, func, sym_list->symbol->name, st_PARAM);
            propogateSymbol(func, ste);
        } else {
            ste->symbol->flags = st_ERROR;
        }
    }
}

DEFINE_PASS(build_symbols_locals) {
    function_t* func = *func_ptr;
    LOG_PASS(build_symbols_locals, func);

    symbol_list_t* sym_list = NULL;
    DL_FOREACH(func->locals, sym_list) {
        symbol_table_entry_t* ste =
            hasSymbol(module, func, sym_list->symbol->name);
        if(ste == NULL) {
            ste = insertSymbol(module, func, sym_list->symbol->name, st_LOCAL);
            propogateSymbol(func, ste);

        } else {
            ste->symbol->flags = st_ERROR;
        }
    }
}


void findCallInStatement(ast_stmt_t* stmt, module_t* module);
void findCallInStatements(ast_stmt_t* stmt, module_t* module);
DEFINE_PASS(build_symbols_calls) {
    function_t* func = *func_ptr;
    LOG_PASS(build_symbols_calls, func);
    findCallInStatements(func->stmts, module);
}

void findCallInStatement(ast_stmt_t* stmt, module_t* module) {
    if(stmt == NULL)
        return;

    // if the names match, set the symbol
    if(stmt->type == ast_CALL && stmt->left != NULL && stmt->left->type == ast_SYMBOL) {
        //find the function
        symbol_table_entry_t* ste = hasSymbol(module, NULL, stmt->left->data.symbol->name);
        if(ste == NULL) {
            stmt->left->data.symbol->flags = st_ERROR;
        }
        else {
            stmt->left->data.symbol = ste->symbol;
        }
    }

    if(stmt->left != NULL) {
        findCallInStatements(stmt->left, module);
    }
    if(stmt->right != NULL) {
        findCallInStatements(stmt->right, module);
    }
}

// returns id of list head
void findCallInStatements(ast_stmt_t* stmts, module_t* module) {
    ast_stmt_t* temp = NULL;
    LL_FOREACH(stmts, temp) { findCallInStatement(temp, module); }
}
