
#include "ir_interp.h"
#include "common/utlist.h"
#include <stdlib.h>

long get_value(ir_operand_t* oper, long* memory) {
    if(oper->type == ir_IMMEDIATE) return oper->immediate;
    else if(oper->type == ir_VIRTUAL_REGISTER)
        return memory[oper->vregister->reg];
    else {
        fprintf(stderr, "Unknown value operand\n");
        exit(1);
    }
}

ir_instruction_t* execute_inst(FILE* fp, ir_instruction_t* inst, long* memory) {
    ir_instruction_t* next = inst->next;
    ir_operand_t** opers = inst->operands;
    switch(inst->opcode) {
        case ir_COPY: {
            vregister_t dest_addr = opers[0]->vregister->reg;
            memory[dest_addr] = get_value(opers[1], memory);
            break;
        }
        case ir_ADD: {
            vregister_t dest_addr = opers[0]->vregister->reg;
            memory[dest_addr] =
                get_value(opers[1], memory) + get_value(opers[2], memory);
            break;
        }
        case ir_SUB: {
            vregister_t dest_addr = opers[0]->vregister->reg;
            memory[dest_addr] =
                get_value(opers[1], memory) - get_value(opers[2], memory);
            break;
        }
        // equailty, 1 if equal, 0 if not
        case ir_CMP: {
            vregister_t dest_addr = opers[0]->vregister->reg;
            memory[dest_addr] =
                get_value(opers[1], memory) == get_value(opers[2], memory);
            break;
        }
        case ir_JMP: next = opers[0]->target; break;
        // jump if zero
        case ir_CJMP: {
            vregister_t dest_addr = opers[0]->vregister->reg;
            if(memory[dest_addr] == 0) {
                next = opers[1]->target;
            }
            break;
        }
        case ir_CALL:
        case ir_RET: break;

        case ir_PRINT: {
            fprintf(fp, "%ld\n", get_value(opers[0], memory));
            break;
        }
        case ir_NOP:
        default: break;
    }
    return next;
}

void _execute_ir(FILE* fp, ir_function_t* function, long* memory) {
    ir_instruction_t* inst = function->ir;
    while(inst != NULL) {
        // if execute instruction returns a new instruction, use it
        // otherwise, just go the end of list
        ir_instruction_t* temp = execute_inst(fp, inst, memory);
        if(temp) inst = temp;
        else inst = inst->next;
    }
}

long* allocate_memory(ir_function_t* function) {
    int n_vregisters = function->mm->next_reg + 1;
    long* memory = malloc(n_vregisters * sizeof(*memory));
    return memory;
}

void execute_ir(FILE* fp, ir_function_t* function) {
    long* memory = allocate_memory(function);
    _execute_ir(fp, function, memory);
}
