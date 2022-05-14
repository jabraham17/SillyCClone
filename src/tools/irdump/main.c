#include "common/args/arg_creation.h"
#include "common/args/file_helper.h"
#include "common/args/string_list.h"

#include "common/utlist.h"
#include "ir/build_ir.h"
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

// iterate to begining by counting moving prev point until prev->next is NULL
// not terrible efficient, but it works
int get_inst_idx(ir_instruction_t* inst) {
    int count = 0;
    ir_instruction_t* temp = inst;
    while(temp->prev->next != NULL) {
        count++;
        temp = temp->prev;
    }
    return count;
}

void dump_operand(FILE* fp, ir_operand_t* oper) {
    switch(oper->type) {
        case ir_IMMEDIATE: fprintf(fp, "%-6ld", oper->immediate); break;
        case ir_VIRTUAL_REGISTER:
            fprintf(fp, "%%%-5ld", oper->vregister->reg);
            break;
        case ir_JMP_TARGET:
            fprintf(fp, "@%-5d", get_inst_idx(oper->target));
            break;
        case ir_CALL_TARGET: fprintf(fp, "@%-5s", oper->function->name); break;
        default: fprintf(fp, "UNKWN ");
    }
}

void dump_instruction(FILE* fp, ir_instruction_t* inst) {
    if(!inst->isJmpTrgt) fprintf(fp, "    ");
    else fprintf(fp, "%2d: ", get_inst_idx(inst));

    fprintf(fp, "%-6s", getIROpcodeString(inst->opcode));
    for(int i = 0; inst->operands[i] != NULL; i++) {
        dump_operand(fp, inst->operands[i]);
    }
    fprintf(fp, "\n");
}
void dump_function(FILE* fp, ir_function_t* func) {
    fprintf(fp, "FUNC %s:\n", func->name);
    ir_instruction_t* elm;
    DL_FOREACH(func->ir, elm) { dump_instruction(fp, elm); }
}

void dump_symbol(FILE* fp, ir_register_t* reg) {
    char* symName = reg->symbol ? reg->symbol->name : "temp";
    fprintf(fp, "    %%%-5ld = %s\n", reg->reg, symName);
}

void dump_symbols(FILE* fp, ir_function_t* func) {
    fprintf(fp, "SYMBOLS FUNC %s:\n", func->name);
    ir_register_list_t* elm;
    DL_FOREACH(func->mm->registers, elm) { dump_symbol(fp, elm->ir_register); }
}
void dump_functions(FILE* fp, ir_function_list_t* funcs, int dumpSymbols) {
    fprintf(fp, "MODULE\n\n");
    ir_function_list_t* elm;
    // dirty trick, since I cant have an empty string
    // instead I can have a str of length 1, then either contains the null char
    // or it contains the seperator
    // when it contains the null char, it appears as a zero length string
    // genius!
    char sep[2] = {'\0','\0'};
    DL_FOREACH(funcs, elm) {
        fputs(sep, fp);
        sep[0] = '\n';

        dump_function(fp, elm->ir_function);
        if(dumpSymbols) dump_symbols(fp, elm->ir_function);
    }
}

int str_in_strarr(char* str, char** array) {
    while(*array != NULL) {
        if(strcmp(str, *array) == 0) {
            return 1;
        }
        array++;
    }
    return 0;
}

int _debug_mode = 0;
int main(int argc, char** argv) {

    char* inFileName = NULL;
    char* outFileName = NULL;
    int dumpSymbols = 0;
    SL_INIT(functionNamesLL);

    #define add_file_to_filter SL_ADD(functionNamesLL, optarg);

#define ARGS(WITH_ARG, WITH_BOOL, WITH_CUSTOM)                                 \
    WITH_ARG(i, inFileName)                                                    \
    WITH_ARG(o, outFileName)                                                   \
    WITH_BOOL(s, dumpSymbols)                                                  \
    WITH_BOOL(d, _debug_mode)                                                  \
    WITH_CUSTOM(f, add_file_to_filter, ":")

    MAKE_ARGS(argc, argv, ARGS);


    FILE* inFile = NULL;
    OPEN_FILE_OR_STDIN(inFile, inFileName);

    // char** functionNames = ll_to_array(functionNames_ll);
    SL_ARRAY(functionNamesLL, functionNames);

    pt_t* root = parse(inFile);
    fclose(inFile);
    module_t* module = pt_to_ast(root);
    runPass(module, buildPass(PASS_build_symbol_table));
    ir_function_list_t* ir_funcs = ast_to_ir(module);

    // only want to filter out after processing, before printing
    // this way external references to other functions not included still work
    if(functionNames) {
        ir_function_list_t* filteredFuncs = NULL;
        ir_function_list_t* elm = NULL;
        ir_function_list_t* temp = NULL;
        DL_FOREACH_SAFE(ir_funcs, elm, temp) {
            if(str_in_strarr(elm->ir_function->name, functionNames)) {
                DL_DELETE(ir_funcs, elm);
                DL_APPEND(filteredFuncs, elm);
            }
        }
        ir_funcs = filteredFuncs;
    }

    FILE* outFile = NULL;
    OPEN_FILE_OR_STDOUT(outFile, outFileName);

    dump_functions(outFile, ir_funcs, dumpSymbols);
    fclose(outFile);

    return 0;
}
