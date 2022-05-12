
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
        case ir_CALL_TARGET:
            fprintf(fp, "@%-5s", oper->function->name);
            break;
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
    DL_FOREACH(func->ir, elm) {
        dump_instruction(fp, elm);
    }
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
    char sep = '\0';
    DL_FOREACH(funcs, elm) {
        fputc(sep, fp);
        sep = '\n';

        dump_function(fp, elm->ir_function);
        if(dumpSymbols) dump_symbols(fp, elm->ir_function);
    }
}

int _debug_mode = 0;

int main(int argc, char** argv) {

    char* inFileName = NULL;
    char* outFileName = NULL;
    int dumpSymbols = 0;
    char c;
    opterr = 1;
    while((c = getopt(argc, argv, "i:o:sd")) != -1) {
        switch(c) {
            case 'i': inFileName = optarg; break;
            case 'o': outFileName = optarg; break;
            case 'd': _debug_mode = 1; break;
            case 's': dumpSymbols = 1; break;
            case '?':
            default: return 1;
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
        return -1;
    }

    dump_functions(outFile, ir_funcs, dumpSymbols);
    fclose(outFile);

    return 0;
}
