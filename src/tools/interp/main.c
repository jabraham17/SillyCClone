#include "common/args/arg_creation.h"
#include "common/args/file_helper.h"
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

#define ARGS(WITH_ARG, WITH_BOOL, WITH_CUSTOM)                                   \
    WITH_ARG(i, inFileName)                                                    \
    WITH_ARG(o, outFileName)                                                   \
    WITH_ARG(e, entryFuncName)                                                 \
    WITH_BOOL(d, _debug_mode)

    MAKE_ARGS(argc, argv, ARGS);

    if(!entryFuncName) {
        entryFuncName = strdup("main");
    }

    FILE* inFile = NULL;
    OPEN_FILE_OR_STDIN(inFile, inFileName);

    pt_t* root = parse(inFile);
    fclose(inFile);
    module_t* module = pt_to_ast(root);
    runPass(module, buildPass(PASS_build_symbol_table));
    ir_function_list_t* ir_funcs = ast_to_ir(module);

    FILE* outFile = NULL;
    OPEN_FILE_OR_STDOUT(outFile, outFileName);

    ir_function_t* entryFunc = get_entry_func(entryFuncName, ir_funcs);
    if(!entryFunc) {
        fprintf(stderr, "No entry function '%s' found\n", entryFuncName);
        return 1;
    }

    execute_ir(outFile, entryFunc);
    fclose(outFile);

    return 0;
}
