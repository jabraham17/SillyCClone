
#include "parser/parser.h"
#include "sema/ast/ast.h"
#include "sema/ast/pt_conversion.h"
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sema/pass_manager/pass_manager.h"

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

intptr_t statementsToDot(ast_stmt_t* stmts, FILE* f);

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
        case ast_IF:
        case ast_WHILE:
        case ast_RETURN:
        case ast_CALL:
            fprintf(f, "[label=\"%s\"];\n", getASTTypeString(stmt->type));
            break;

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
        statementsToDot(stmt->left, f);
    }
    if(stmt->right != NULL) {
        fprintf(
            f, "stmt_%ld->stmt_%ld;\n", (intptr_t)(stmt),
            (intptr_t)(stmt->right));
        statementsToDot(stmt->right, f);
    }
}

// returns id of list head
intptr_t statementsToDot(ast_stmt_t* stmts, FILE* f) {
    intptr_t headId = (intptr_t)stmts;
    intptr_t prevId = 0;
    ast_stmt_t* temp = stmts;
    while(temp != NULL) {
        statementToDot(temp, f);
        if(prevId) {
            fprintf(
                f, "stmt_%ld->stmt_%ld[label=next;color=crimson];\n", prevId,
                (intptr_t)(temp));
        }
        prevId = (intptr_t)(temp);
        temp = temp->next;
    }
    return headId;
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
    if(func->locals) {
        variableListToDot("locals", func->locals, f);
        fprintf(
            f, "func_%ld->varList_%ld;\n", (intptr_t)func,
            (intptr_t)func->locals);
    }
    if(func->stmts) {
        intptr_t id = statementsToDot(func->stmts, f);
        if(id) {
            fprintf(f, "func_%ld->stmt_%ld;\n", (intptr_t)(func), (intptr_t)id);
        }
    }

    fprintf(f, "}\n");
}

int _debug_mode = 0;

int main(int argc, char** argv) {

    char* inFileName = NULL;
    char* outFileName = NULL;
    char c;
    opterr = 1;
    while((c = getopt(argc, argv, "i:o:d")) != -1) {
        switch(c) {
            case 'i':
                inFileName = optarg;
                break;
            case 'o':
                outFileName = optarg;
                break;
            case 'd':
                _debug_mode = 1;
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


    module_t* module = pt_to_ast(root);

    //pass_t* p = chainPass(buildPass(PASS_silly_pass), buildPass(PASS_silly_pass2), NULL);

    runPass(module, buildPass(PASS_all_silly));

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
    function_list_t* functions = module->functions;
    function_list_t* temp = functions;
    while(temp != NULL) {
        functionToDot(temp->function, outFile);
        temp = temp->next;
    }
    fprintf(outFile, "}\n");

    fclose(outFile);

    return 0;
}
