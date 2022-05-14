
#include "common/args/arg_creation.h"
#include "common/args/file_helper.h"
#include "parser/parser.h"
#include "parser/parsetree.h"
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void _ptToDot(pt_t* pt, FILE* f) {
    if(pt == NULL) return;

    switch(pt->type) {
        case pt_NONE: {
            fprintf(f, "%ld[label=\"\"];\n", (intptr_t)pt);
            break;
        }
        case pt_NUM: {
            fprintf(f, "%ld[label=\"%d\"];\n", (intptr_t)pt, pt->data.num);
            break;
        }
        case pt_STR: {
            fprintf(f, "%ld[label=\"%s\"];\n", (intptr_t)pt, pt->data.str);
            break;
        }
        case pt_PLUS:
        case pt_MINUS:
        case pt_DEQUALS:
        case pt_EQUALS:
        case pt_IF:
        case pt_WHILE:
        case pt_RETURN:
        case pt_FUNCTION_DEF:
        case pt_CALL:
        case pt_PRINT: {
            fprintf(
                f, "%ld[label=\"%s\"];\n", (intptr_t)pt,
                getPTTypeString(pt->type));
            break;
        }

        case pt_PARAM_DEF:
        case pt_VAR_DEF:
        case pt_PARAM_LIST: {
            fprintf(
                f, "%ld[label=\"%s", (intptr_t)pt, getPTTypeString(pt->type));
            char** strs = pt->data.ids;
            size_t idx = 0;
            while(strs != NULL && strs[idx] != NULL) {
                fprintf(f, "\\n%s", strs[idx]);
                idx++;
            }
            fprintf(f, "\"];\n");
            break;
        }
        default: fprintf(stderr, "Unknown pt type\n"); break;
    }
    size_t idx = 0;
    while(pt->children[idx] != NULL) {
        fprintf(f, "%ld->%ld;\n", (intptr_t)pt, (intptr_t)(pt->children[idx]));
        _ptToDot(pt->children[idx], f);
        idx++;
    }
    if(pt->next != NULL) {
        fprintf(
            f, "%ld->%ld[label=next];\n", (intptr_t)pt, (intptr_t)(pt->next));
        _ptToDot(pt->next, f);
    }
}
void ptToDot(pt_t* pt, FILE* f) {
    fprintf(f, "digraph pt {\n");
    _ptToDot(pt, f);
    fprintf(f, "}\n");
}

int _debug_mode = 0;

int main(int argc, char** argv) {

    char* inFileName = NULL;
    char* outFileName = NULL;
#define ARGS(WITH_ARG, WITH_BOOL, WITH_CUSTOM)                                 \
    WITH_ARG(i, inFileName)                                                    \
    WITH_ARG(o, outFileName)                                                   \
    WITH_BOOL(d, _debug_mode)

    MAKE_ARGS(argc, argv, ARGS);

    FILE* inFile = NULL;
    OPEN_FILE_OR_STDIN(inFile, inFileName);
    pt_t* root = parse(inFile);
    fclose(inFile);

    FILE* outFile = NULL;
    OPEN_FILE_OR_STDOUT(outFile, outFileName);
    ptToDot(root, outFile);
    fclose(outFile);

    return 0;
}
