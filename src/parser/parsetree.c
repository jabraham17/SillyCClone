#include "parsetree.h"
#include <stdlib.h>
#include <string.h>

struct id_list {
    char* id;
    size_t count;
    id_list_t* next;
};

id_list_t* getIDNode(char* id, id_list_t* oldList) {
    id_list_t* node = malloc(sizeof(*node));
    node->id = id;
    node->next = oldList;
    node->count = (oldList != NULL) ? (1 + oldList->count) : 1;
    return node;
}

char* getPTTypeString(pt_type_t t) {
    switch(t) {
#define PT_CASE(_NAME)                                                        \
    case pt_##_NAME:                                                          \
        return #_NAME;
        _PT_TYPES(PT_CASE)
#undef PT_CASE
    }
    return NULL;
}

pt_t* allocatePT() { return allocatePT_TYPE(pt_NONE); }
pt_t* allocatePT_TYPE(pt_type_t t) {
    pt_t* pt = malloc(sizeof(*pt));
    pt->type = t;
    memset(&(pt->data), 0, sizeof(pt->data));
    memset(pt->children, 0, sizeof(pt->children));
    return pt;
}
pt_t* allocatePT_NUM(int num) {
    pt_t* pt = allocatePT_TYPE(pt_NUM);
    pt->data.num = num;
    return pt;
}
pt_t* allocatePT_STR(char* str) {
    pt_t* pt = allocatePT_TYPE(pt_STR);
    pt->data.str = str;
    return pt;
}
pt_t* allocatePT_ID_LIST(pt_type_t t, id_list_t* id_list) {
    // first elm should have max number of ids
    // need to allocate one extra for NUm
    size_t nElms = id_list->count;
    char** ids = malloc(sizeof(*ids) * (nElms + 1));
    size_t idx = 0;
    while(id_list != NULL) {
        ids[idx] = id_list->id;
        id_list = id_list->next;
        idx++;
    }
    ids[nElms] = NULL;

    pt_t* pt = allocatePT_TYPE(t);
    pt->data.ids = ids;
    return pt;
}
void addChild(pt_t* parent, pt_t* child) {
    for(size_t i = 0; i < _PT_MAX_CHILDREN; i++) {
        if(parent->children[i] == NULL) {
            parent->children[i] = child;
            break;
        }
    }
}

// statement_t* createFunctionCallStatement(symbol_t* sym, symbol_list_t*
// sym_list) {
//     statement_t* stmt = (statement_t*)malloc(sizeof(statement_t));
//     stmt->tt = st_FUNCTION_CALL;
//     stmt->sym = sym;
//     stmt->expr = NULL;
//     stmt->scope = NULL;
//     stmt->sym_list = NULL;
// }
