
#include "parser/parser.h"
#include "sema/ast/ast.h"
#include "sema/ast/pt_conversion.h"
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void variableListToDot(char* name, variable_list_t* var_list, FILE* f) {
    fprintf(f, "varList_%ld[label=\"%s\"];\n", (intptr_t)var_list, name);
    variable_list_t* temp = var_list;
    while(temp != NULL) {
        fprintf(
            f, "var_%ld_%ld[label=\"%s\"];\n", (intptr_t)(var_list),
            (intptr_t)(temp->variable), temp->variable->name);
        fprintf(
            f, "varList_%ld->var_%ld_%ld;\n", (intptr_t)var_list,
            (intptr_t)(var_list), (intptr_t)(temp->variable));
        temp = temp->next;
    }
}

void scopeToDot(scope_t* scope, FILE* f);

void statementToDot(ast_stmt_t* stmt, FILE* f) {
    if(stmt == NULL)
        return;
    fprintf(f, "stmt_%ld", (intptr_t)(stmt));
    switch(stmt->type) {
        case ast_NUMBER:
            fprintf(f, "[label=\"%d\"];\n", stmt->data.num);
            break;

        case ast_VARIABLE:
            fprintf(f, "[label=\"%s\"];\n", stmt->data.variable->name);
            break;
        case ast_EQUALS:
        case ast_DEQUALS:
        case ast_PLUS:
        case ast_MINUS:
            fprintf(f, "[label=\"%s\"];\n", getASTTypeString(stmt->type));
            break;
        case ast_SCOPE:
            fprintf(f, "[label=\"scope\"];\n");
            scopeToDot(stmt->data.scope, f);
            fprintf(
                f, "stmt_%ld->scope_%ld;\n", (intptr_t)(stmt),
                (intptr_t)(stmt->data.scope));
            break;
        case ast_IF:
        case ast_WHILE:
        case ast_RETURN:
        case ast_CALL:

        case ast_NOP:
            fprintf(f, ";\n");
            fprintf(
                stderr, "Unhandled statement type %s\n",
                getASTTypeString(stmt->type));
            break;
    }

    if(stmt->left != NULL) {
        fprintf(
            f, "stmt_%ld->stmt_%ld;\n", (intptr_t)(stmt),
            (intptr_t)(stmt->left));
        statementToDot(stmt->left, f);
    }
    if(stmt->right != NULL) {
        fprintf(
            f, "stmt_%ld->stmt_%ld;\n", (intptr_t)(stmt),
            (intptr_t)(stmt->right));
        statementToDot(stmt->right, f);
    }
}

// returns id of list head
intptr_t statementsToDot(ast_stmt_list_t* stmts, FILE* f) {
    intptr_t headId = (intptr_t)stmts->ast_stmt;
    intptr_t prevId = 0;
    ast_stmt_list_t* temp = stmts;
    while(temp != NULL) {
        fprintf(f, "subgraph cluster_stmt_%ld {\n", (intptr_t)(temp->ast_stmt));
        statementToDot(temp->ast_stmt, f);
        if(prevId) {
            fprintf(
                f, "stmt_%ld->stmt_%ld;\n", prevId, (intptr_t)(temp->ast_stmt));
        }
        fprintf(f, "}\n");
        prevId = (intptr_t)(temp->ast_stmt);
        temp = temp->next;
    }
    return headId;
}

void scopeToDot(scope_t* scope, FILE* f) {

    fprintf(
        f, "subgraph cluster_scope_%ld {\nlabel=\"\";rankdir=TB;\n\n",
        (intptr_t)scope);

    fprintf(f, "scope_%ld[label=\"\",shape=point];\n", (intptr_t)scope);

    variableListToDot("locals", scope->variables, f);
    fprintf(
        f, "scope_%ld->varList_%ld;\n", (intptr_t)scope,
        (intptr_t)scope->variables);

    intptr_t id = statementsToDot(scope->statements, f);
    if(id) {
        fprintf(f, "scope_%ld->stmt_%ld;\n", (intptr_t)scope, (intptr_t)id);
    }

    fprintf(f, "}\n");

    // if it has a parent scopem, make it a child of that
    if(scope->parent_scope) {
        fprintf(
            f, "scope_%ld->scope_%ld;\n", (intptr_t)scope->parent_scope,
            (intptr_t)scope);
    }
    // direct child of function
    else {
        fprintf(
            f, "func_%ld->scope_%ld;\n", (intptr_t)scope->function,
            (intptr_t)scope);
    }
}

void functionToDot(function_t* func, FILE* f) {
    fprintf(
        f, "subgraph cluster_func_%ld {\nlabel=\"function: %s\";\n",
        (intptr_t)func, func->name);

    fprintf(f, "func_%ld[label=\"%s\"];\n", (intptr_t)func, func->name);

    if(func->params) {
        variableListToDot("params", func->params, f);
        fprintf(
            f, "func_%ld->varList_%ld;\n", (intptr_t)func,
            (intptr_t)func->params);
    }

    scopeToDot(func->scope, f);

    fprintf(f, "}\n");
}

int main(int argc, char** argv) {

    char* inFileName = NULL;
    char* outFileName = NULL;
    char c;
    opterr = 1;
    while((c = getopt(argc, argv, "i:o:")) != -1) {
        switch(c) {
            case 'i':
                inFileName = optarg;
                break;
            case 'o':
                outFileName = optarg;
                break;
            case '?':
            default:
                return 1;
        }
    }

    FILE* inFile = NULL;
    if(inFileName != NULL) {
        inFile = fopen(inFileName, "r");
    } else {
        int stdinDup = dup(fileno(stdin));
        inFile = fdopen(stdinDup, "r");
    }
    if(!inFile) {
        fprintf(stderr, "Invalid input file\n");
        return -1;
    }

    pt_t* root = parse(inFile);
    fclose(inFile);

    FILE* outFile = NULL;
    if(outFileName != NULL) {
        outFile = fopen(outFileName, "w");
    } else {
        int stdoutDup = dup(fileno(stdout));
        outFile = fdopen(stdoutDup, "w");
    }
    if(!outFile) {
        fprintf(stderr, "Invalid output file\n");
        return -1;
    }

    fprintf(outFile, "digraph ast {\n");
    fprintf(outFile, "compound=true;\n");
    function_list_t* functions = pt_to_ast(root);
    function_list_t* temp = functions;
    while(temp != NULL) {
        functionToDot(temp->function, outFile);
        temp = temp->next;
    }
    fprintf(outFile, "}\n");

    fclose(outFile);

    return 0;
}
