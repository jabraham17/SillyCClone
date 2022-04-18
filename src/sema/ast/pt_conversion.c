#include "pt_conversion.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

variable_list_t* get_var_list(pt_t* pt_var_list, scope_t* parent) {
    assert(
        pt_var_list != NULL &&
        (pt_var_list->type == pt_PARAM_DEF || pt_var_list->type == pt_VAR_DEF ||
         pt_var_list->type == pt_PARAM_LIST));
    variable_list_t dummy;
    variable_list_t* head = &dummy;
    head->next = NULL;
    head->prev = NULL;

    variable_list_t* temp = head;
    char** strs = pt_var_list->data.ids;
    size_t idx = 0;
    while(strs != NULL && strs[idx] != NULL) {

        // add new list element to end
        variable_list_t* newList = malloc(sizeof(*newList));
        temp->next = newList;
        newList->prev = temp;
        newList->next = NULL;
        temp = newList;

        // add variable to the new list element
        temp->variable = malloc(sizeof(*(temp->variable)));
        temp->variable->name = strs[idx];
        temp->variable->parent_scope = parent;

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

int is_bin_expr_type(pt_type_t t) {
    return t == pt_PLUS || t == pt_MINUS || t == pt_DEQUALS || t == pt_EQUALS;
}
int is_leaf_expr_type(pt_type_t t) { return t == pt_STR || t == pt_NUM; }
int is_expr_type(pt_type_t t) {
    return is_bin_expr_type(t) || is_leaf_expr_type(t) || t == pt_CALL;
}

ast_stmt_t* get_expr(pt_t* pt_expr) {
    assert(pt_expr != NULL && is_expr_type(pt_expr->type));
    // we do not handle next nodes in the expr tree

    if(is_bin_expr_type(pt_expr->type)) {
        ast_stmt_t* expr = malloc(sizeof(*expr));
        expr->type = get_bin_ast_type(pt_expr->type);
        expr->left = get_expr(pt_expr->children[0]);
        expr->right = get_expr(pt_expr->children[1]);
        return expr;
    }
    if(pt_expr->type == pt_NUM) {
        ast_stmt_t* expr = malloc(sizeof(*expr));
        expr->type = ast_NUMBER;
        expr->left = NULL;
        expr->right = NULL;
        expr->data.num = pt_expr->data.num;
        return expr;
    }
    if(pt_expr->type == pt_STR) {
        ast_stmt_t* expr = malloc(sizeof(*expr));
        expr->type = ast_VARIABLE;
        expr->left = NULL;
        expr->right = NULL;
        // create dummy variable
        expr->data.variable = malloc(sizeof(*expr->data.variable));
        expr->data.variable->name = pt_expr->data.str;
        expr->data.variable->parent_scope = NULL;

        return expr;
    }
    if(pt_expr->type == pt_CALL) {
        return NULL;
    }

    return NULL;
}

scope_t*
get_scope(pt_t* pt_scope, function_t* parent_func, scope_t* parent_scope);

ast_stmt_t*
get_stmt(pt_t* pt_stmt, function_t* parent_func, scope_t* parent_scope) {
    assert(pt_stmt != NULL);
    // STMT has:
    // child 0..n: specific stmt data

    if(is_expr_type(pt_stmt->type)) {
        return get_expr(pt_stmt);
    }
    if(pt_stmt->type == pt_SCOPE) {
        ast_stmt_t* stmt = malloc(sizeof(*stmt));
        stmt->type = ast_SCOPE;
        stmt->left = NULL;
        stmt->right = NULL;
        stmt->data.scope = get_scope(pt_stmt, parent_func, parent_scope);
        return stmt;
    }
    return NULL;
}

ast_stmt_list_t*
get_stmts(pt_t* pt_stmt, function_t* parent_func, scope_t* parent_scope) {
    assert(pt_stmt != NULL);
    // STMT has:
    // child 0..n: specific stmt data

    ast_stmt_list_t dummy;
    ast_stmt_list_t* head = &dummy;
    head->next = NULL;
    head->prev = NULL;

    ast_stmt_list_t* temp = head;
    while(pt_stmt != NULL) {
        // add new list element to end
        ast_stmt_list_t* newList = malloc(sizeof(*newList));
        temp->next = newList;
        newList->prev = temp;
        newList->next = NULL;
        temp = newList;

        // add stmt to the new list element
        temp->ast_stmt = get_stmt(pt_stmt, parent_func, parent_scope);

        pt_stmt = pt_stmt->next;
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

// parent scope may be null, in which case this is a top level scope
scope_t*
get_scope(pt_t* pt_scope, function_t* parent_func, scope_t* parent_scope) {
    assert(pt_scope != NULL && pt_scope->type == pt_SCOPE);
    // we have a function def
    // child 0: var_def
    // child 1: statement list

    scope_t* scope = malloc(sizeof(*scope));
    scope->parent_scope = parent_scope;
    scope->function = parent_func;
    scope->variables = get_var_list(pt_scope->children[0], scope);
    scope->statements = get_stmts(pt_scope->children[1], parent_func, scope);

    return scope;
}

function_t* get_function(pt_t* pt_name, pt_t* pt_params, pt_t* pt_scope) {
    assert(pt_name != NULL && pt_name->type == pt_STR);
    assert(pt_params != NULL && pt_params->type == pt_PARAM_DEF);
    assert(pt_scope != NULL && pt_scope->type == pt_SCOPE);

    function_t* func = malloc(sizeof(*func));
    func->name = pt_name->data.str;
    func->scope = get_scope(pt_scope, func, NULL);
    func->params = get_var_list(pt_params, func->scope);

    return func;
}

function_list_t* pt_to_ast(pt_t* pt) {
    if(pt == NULL || pt->type != pt_FUNCTION_DEF)
        return NULL;

    // we have a function def
    // child 0: name
    // child 1: parameters
    // child 2: scope
    // next: next function
    function_t* func =
        get_function(pt->children[0], pt->children[1], pt->children[2]);

    function_list_t* list = malloc(sizeof(*list));
    list->function = func;
    list->next = pt_to_ast(pt->next);
    list->prev = NULL;
    // if there are more functions, need to link in prev
    if(list->next != NULL)
        list->next->prev = list;

    return list;
}
