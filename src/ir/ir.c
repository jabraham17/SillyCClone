
#include "ir.h"
#include "common/utlist.h"
#include <stdarg.h>
#include <stdlib.h>

char* getIROpcodeString(ir_opcode_t op) {
    switch(op) {
#define IR_CASE(_NAME)                                                         \
    case ir_##_NAME: return #_NAME;

        _IR_OPCODES(IR_CASE)
#undef IR_CASE
    }
    return "";
}

ir_operand_t* ir_build_operand_vregister(ir_register_t* vregister) {
    ir_operand_t* oper = malloc(sizeof(*oper));
    oper->type = ir_VIRTUAL_REGISTER;
    oper->vregister = vregister;
    return oper;
}
ir_operand_t* ir_build_operand_immediate(long immediate) {
    ir_operand_t* oper = malloc(sizeof(*oper));
    oper->type = ir_IMMEDIATE;
    oper->immediate = immediate;
    return oper;
}
ir_operand_t* ir_build_operand_target(ir_instruction_t* target) {
    ir_operand_t* oper = malloc(sizeof(*oper));
    oper->type = ir_JMP_TARGET;
    oper->target = target;
    target->isJmpTrgt = 1;
    return oper;
}

ir_operand_t** vir_build_operands(unsigned int noperands, va_list args) {
    ir_operand_t** operands = malloc((noperands+1) * sizeof(*operands));
    for(size_t i = 0; i < noperands; i++) {
        operands[i] = va_arg(args, ir_operand_t*);
    }
    operands[noperands] = NULL; // last elm NULL
    return operands;
}

ir_instruction_t* ir_build_instruction(ir_opcode_t opcode, ...) {
    // count number of args
    size_t count = 0;
    va_list args;
    va_start(args, opcode);
    while(1) {
        ir_operand_t* elm = va_arg(args, ir_operand_t*);
        if(elm == NULL) break;
        count++;
    }
    va_end(args);

    // allocate and set arguments
    va_start(args, opcode);
    ir_operand_t** operands = vir_build_operands(count, args);
    va_end(args);

    return ir_build_instruction_with_operands(opcode, operands);
}
ir_instruction_t* ir_build_instruction_with_operands(
    ir_opcode_t opcode, ir_operand_t** operands) {
    ir_instruction_t* inst = malloc(sizeof(*inst));
    inst->opcode = opcode;
    inst->operands = operands;
    inst->isJmpTrgt = 0;
    inst->next = NULL;
    inst->prev = NULL;
    return inst;
}
ir_operand_t** ir_build_operands(unsigned int noperands, ...) {
    va_list args;
    va_start(args, noperands);
    ir_operand_t** operands = vir_build_operands(noperands, args);
    va_end(args);
    return operands;
}

ir_memorymap_t* ir_init_mm() {
    ir_memorymap_t* mm = malloc(sizeof(*mm));
    mm->next_reg = 0;
    mm->registers = NULL;
    return mm;
}

ir_register_t* alloc_register(symbol_t* sym, vregister_t regID) {
    ir_register_t* reg = malloc(sizeof(*reg));
    reg->symbol = sym;
    reg->reg = regID;
    return reg;
}
ir_register_list_t* alloc_register_list(ir_register_t* reg) {
    ir_register_list_t* elm = malloc(sizeof(*elm));
    elm->ir_register = reg;
    elm->next = NULL;
    elm->prev = NULL;
    return elm;
}

// allocate for a symbol
ir_register_t* ir_get_irreg(ir_memorymap_t* mm, symbol_t* sym) {

    // look for register for symbol
    ir_register_list_t* elm = NULL;
    DL_FOREACH(mm->registers, elm) {
        if(elm->ir_register->symbol == sym) {
            return elm->ir_register;
        }
    }

    // no existing register, allocate one
    ir_register_t* next = alloc_register(sym, mm->next_reg);
    mm->next_reg++;

    elm = alloc_register_list(next);
    DL_APPEND(mm->registers, elm);
    return next;
}
// allocate a temporary
ir_register_t* ir_get_temp_irreg(ir_memorymap_t* mm) {
    ir_register_t* next = alloc_register(NULL, mm->next_reg);
    mm->next_reg++;

    ir_register_list_t* elm = alloc_register_list(next);
    DL_APPEND(mm->registers, elm);
    return next;
}
