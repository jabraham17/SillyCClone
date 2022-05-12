#include "common/utlist.h"
#include "ir/build_ir.h"
#include "ir_interp.h"
#include "parser/parser.h"
#include "sema/ast/ast.h"
#include "sema/ast/pt_conversion.h"
#include "sema/pass_manager/pass_manager.h"
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

ir_function_t* get_entry_func(char* entryName, ir_function_list_t* funcs) {
    ir_function_list_t* entry = NULL;
    DL_FOREACH(funcs, entry) {
        if(strcmp(entry->ir_function->name, entryName) == 0) {
            return entry->ir_function;
        }
    }
    return NULL;
}

int _debug_mode = 0;

int main(int argc, char** argv) {

    char* inFileName = NULL;
    char* outFileName = NULL;
    char* entryFuncName = NULL;

    char c;
    opterr = 1;
    while((c = getopt(argc, argv, "i:o:e:d")) != -1) {
        switch(c) {
            case 'i': inFileName = optarg; break;
            case 'o': outFileName = optarg; break;
            case 'e': entryFuncName = optarg; break;
            case 'd': _debug_mode = 1; break;
            case '?':
            default: return 1;
        }
    }

    if(!entryFuncName) {
        entryFuncName = strdup("main");
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
        return 1;
    }

    pt_t* root = parse(inFile);
    fclose(inFile);
    module_t* module = pt_to_ast(root);
    runPass(module, buildPass(PASS_build_symbol_table));
    ir_function_list_t* ir_funcs = ast_to_ir(module);

    FILE* outFile = NULL;
    if(outFileName != NULL) {
        outFile = fopen(outFileName, "w");
    } else {
        int stdoutDup = dup(fileno(stdout));
        outFile = fdopen(stdoutDup, "w");
    }
    if(!outFile) {
        fprintf(stderr, "Invalid output file\n");
        return 1;
    }

    ir_function_t* entryFunc = get_entry_func(entryFuncName, ir_funcs);
    if(!entryFunc) {
        fprintf(stderr, "No entry function '%s' found\n", entryFuncName);
        return 1;
    }

    execute_ir(outFile, entryFunc);
    fclose(outFile);

    return 0;
}
