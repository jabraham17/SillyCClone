#include "pt_conversion.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

symbol_list_t* get_sym_list(pt_t* pt_sym_list) {
    assert(
        pt_sym_list != NULL &&
        (pt_sym_list->type == pt_PARAM_DEF || pt_sym_list->type == pt_VAR_DEF ||
         pt_sym_list->type == pt_PARAM_LIST));
    symbol_list_t dummy;
    symbol_list_t* head = &dummy;
    head->next = NULL;
    head->prev = NULL;

    symbol_list_t* temp = head;
    char** strs = pt_sym_list->data.ids;
    size_t idx = 0;
    while(strs != NULL && strs[idx] != NULL) {

        // add new list element to end
        symbol_list_t* newList = malloc(sizeof(*newList));
        temp->next = newList;
        newList->prev = temp;
        newList->next = NULL;
        temp = newList;

        // add symbol to the new list element
        temp->symbol = malloc(sizeof(*(temp->symbol)));
        temp->symbol->name = strs[idx];
        temp->symbol->flags = st_ERROR;

        idx++;
    }

    if(head->next != NULL) {
        // head was a dummy node, lets get rid of it
        // and since the dummy is stack allocated, no free!
        head = head->next;
        head->prev = NULL; // make sure the prev is NULL!!!!
        return head;
    }
    return NULL;
}

ast_stmt_type_t get_bin_ast_type(pt_type_t t) {
    switch(t) {
        case pt_EQUALS:
            return ast_EQUALS;
        case pt_DEQUALS:
            return ast_DEQUALS;
        case pt_PLUS:
            return ast_PLUS;
        case pt_MINUS:
            return ast_MINUS;
        default:
            assert(0);
            return ast_NOP;
    }
}

// last MUST BE NONE
int is_pt_of_type(pt_type_t type, ...) {
    va_list args;
    va_start(args, type);
    while(1) {
        pt_type_t t = va_arg(args, pt_type_t);
        if(t == pt_NONE)
            break;
        if(type == t)
            return 1; // we found one
    }
    va_end(args);
    return 0; // not of type
}

ast_stmt_t* get_str(pt_t* pt_str) {
    assert(pt_str != NULL && pt_str->type == pt_STR);
    ast_stmt_t* expr = malloc(sizeof(*expr));
    expr->type = ast_SYMBOL;
    expr->left = NULL;
    expr->right = NULL;
    expr->next = NULL;
    // create dummy symbol
    expr->data.symbol = malloc(sizeof(*expr->data.symbol));
    expr->data.symbol->name = pt_str->data.str;
    expr->data.symbol->flags = st_ERROR;

    return expr;
}

ast_stmt_t* get_call_stmt(pt_t* pt_call);
ast_stmt_t* get_expr(pt_t* pt_expr) {
    assert(pt_expr != NULL);
    // we do not handle next nodes in the expr tree

    // if(is_bin_expr_type(pt_expr->type)) {
    if(is_pt_of_type(
           pt_expr->type, pt_PLUS, pt_MINUS, pt_DEQUALS, pt_EQUALS, pt_NONE)) {
        ast_stmt_t* expr = malloc(sizeof(*expr));
        expr->type = get_bin_ast_type(pt_expr->type);
        expr->left = get_expr(pt_expr->children[0]);
        expr->right = get_expr(pt_expr->children[1]);
        expr->next = NULL;
        return expr;
    }
    if(pt_expr->type == pt_NUM) {
        ast_stmt_t* expr = malloc(sizeof(*expr));
        expr->type = ast_NUMBER;
        expr->left = NULL;
        expr->right = NULL;
        expr->next = NULL;
        expr->data.num = pt_expr->data.num;
        return expr;
    }
    if(pt_expr->type == pt_STR) {
        return get_str(pt_expr);
    }

    if(pt_expr->type == pt_CALL) {
        return get_call_stmt(pt_expr);
    }

    assert(0);
    return NULL;
}
ast_stmt_t* get_stmts(pt_t* pt_stmt);
ast_stmt_t* get_cond_stmt(pt_t* pt_cond) {
    assert(
        pt_cond != NULL &&
        (pt_cond->type == pt_IF || pt_cond->type == pt_WHILE));

    ast_stmt_t* cond_stmt = malloc(sizeof(*cond_stmt));
    cond_stmt->type = pt_cond->type == pt_IF ? ast_IF : ast_WHILE;
    cond_stmt->left = get_expr(pt_cond->children[0]);
    cond_stmt->right = get_stmts(pt_cond->children[1]);
    cond_stmt->next = NULL;

    return cond_stmt;
}

ast_stmt_t* get_return_stmt(pt_t* pt_ret) {
    assert(pt_ret != NULL && pt_ret->type == pt_RETURN);

    ast_stmt_t* ret_stmt = malloc(sizeof(*ret_stmt));
    ret_stmt->type = ast_RETURN;
    ret_stmt->left = get_expr(pt_ret->children[0]);
    ret_stmt->right = NULL;
    ret_stmt->next = NULL;

    return ret_stmt;
}

ast_stmt_t* get_print_stmt(pt_t* pt_print) {
    assert(pt_print != NULL && pt_print->type == pt_PRINT);

    ast_stmt_t* print_stmt = malloc(sizeof(*print_stmt));
    print_stmt->type = ast_PRINT;
    print_stmt->left = get_expr(pt_print->children[0]);
    print_stmt->right = NULL;
    print_stmt->next = NULL;

    return print_stmt;
}

ast_stmt_t* get_call_stmt(pt_t* pt_call) {
    assert(pt_call != NULL && pt_call->type == pt_CALL);

    ast_stmt_t* call_stmt = malloc(sizeof(*call_stmt));
    call_stmt->type = ast_CALL;
    call_stmt->left = get_str(pt_call->children[0]);
    call_stmt->right = NULL;
    call_stmt->next = NULL;

    symbol_list_t* syms = get_sym_list(pt_call->children[1]);
    ast_stmt_t dummy;
    ast_stmt_t* stmtVarsHead = &dummy;
    stmtVarsHead->left = NULL;
    ast_stmt_t* stmtVars = stmtVarsHead;
    // use left child as 'next' for stmtVars
    while(syms != NULL) {
        stmtVars->left = malloc(sizeof(*stmtVars));
        stmtVars->left->type = ast_SYMBOL;
        stmtVars->left->left = NULL;
        stmtVars->left->right = NULL;
        stmtVars->left->next = NULL;

        // copy pver sym
        stmtVars->left->data.symbol = syms->symbol;

        syms = syms->next;
        stmtVars = stmtVars->left;
    }

    if(stmtVarsHead->left != NULL) {
        // head was a dummy node, lets get rid of it
        // and since the dummy is stack allocated, no free!
        stmtVarsHead = stmtVarsHead->left;
    } else {
        stmtVarsHead = NULL;
    }

    // set right child of call
    call_stmt->right = stmtVarsHead;

    return call_stmt;
}

ast_stmt_t* get_stmt(pt_t* pt_stmt) {
    assert(pt_stmt != NULL);
    // STMT has:
    // child 0..n: specific stmt data

    if(is_pt_of_type(
           pt_stmt->type, pt_PLUS, pt_MINUS, pt_DEQUALS, pt_EQUALS, pt_NUM,
           pt_STR, pt_CALL, pt_NONE)) {
        return get_expr(pt_stmt);
    }
    if(is_pt_of_type(pt_stmt->type, pt_IF, pt_WHILE, pt_NONE)) {
        return get_cond_stmt(pt_stmt);
    }
    if(is_pt_of_type(pt_stmt->type, pt_RETURN, pt_NONE)) {
        return get_return_stmt(pt_stmt);
    }
    if(is_pt_of_type(pt_stmt->type, pt_PRINT, pt_NONE)) {
        return get_print_stmt(pt_stmt);
    }

    assert(0);
    return NULL;
}

ast_stmt_t* get_stmts(pt_t* pt_stmt) {
    assert(pt_stmt != NULL);
    // STMT has:
    // child 0..n: specific stmt data

    ast_stmt_t dummy;
    ast_stmt_t* head = &dummy;
    head->next = NULL;

    ast_stmt_t* temp = head;
    while(pt_stmt != NULL) {
        // get new stmt
        temp->next = get_stmt(pt_stmt);

        pt_stmt = pt_stmt->next;
        temp = temp->next;
    }

    if(head->next != NULL) {
        // head was a dummy node, lets get rid of it
        // and since the dummy is stack allocated, no free!
        head = head->next;
        return head;
    }
    return NULL;
}

function_t*
get_function(pt_t* pt_name, pt_t* pt_params, pt_t* pt_locals, pt_t* pt_stmts) {
    assert(pt_name != NULL && pt_name->type == pt_STR);
    assert(pt_params != NULL && pt_params->type == pt_PARAM_DEF);
    assert(pt_locals != NULL && pt_locals->type == pt_VAR_DEF);
    assert(pt_stmts != NULL);

    function_t* func = malloc(sizeof(*func));

    symbol_t* name = malloc(sizeof(*name));
    name->name = pt_name->data.str;
    name->flags = st_ERROR;
    func->symbol = name;
    
    func->params = get_sym_list(pt_params);
    func->locals = get_sym_list(pt_locals);
    func->stmts = get_stmts(pt_stmts);

    return func;
}

function_list_t* get_functions(pt_t* pt) {
    if(pt == NULL || pt->type != pt_FUNCTION_DEF)
        return NULL;

    // we have a function def
    // child 0: name
    // child 1: parameters
    // child 2: locals
    // child 3: stmts
    // next: next function
    function_t* func = get_function(
        pt->children[0], pt->children[1], pt->children[2], pt->children[3]);

    function_list_t* list = malloc(sizeof(*list));
    list->function = func;
    list->next = get_functions(pt->next);
    list->prev = NULL;
    // if there are more functions, need to link in prev
    if(list->next != NULL)
        list->next->prev = list;

    return list;
}

module_t* pt_to_ast(pt_t* pt) {
    module_t* module = malloc(sizeof(*module));
    module->functions = get_functions(pt);
    module->symbol_table = NULL;
    return module;
}
