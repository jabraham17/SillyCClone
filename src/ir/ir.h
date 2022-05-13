#ifndef _IR_IR_H_
#define _IR_IR_H_

#include "sema/ast/ast.h"

#define _PREDECLARE_STRUCT_TYPE(_name) typedef struct _name _name##_t;
_PREDECLARE_STRUCT_TYPE(ir_function)
_PREDECLARE_STRUCT_TYPE(ir_function_list)
_PREDECLARE_STRUCT_TYPE(ir_operand)
_PREDECLARE_STRUCT_TYPE(ir_instruction)
_PREDECLARE_STRUCT_TYPE(ir_register)
_PREDECLARE_STRUCT_TYPE(ir_register_list)
_PREDECLARE_STRUCT_TYPE(ir_memorymap)
#undef _PREDECLARE_STRUCT_TYPE

#define _MAKE_LIST_FOR_TYPE(_name)                                             \
    struct _name##_list {                                                      \
        _name##_t* _name;                                                      \
        _name##_list_t* next;                                                  \
        _name##_list_t* prev;                                                  \
    };

struct ir_function {
    char* name;
    ir_instruction_t* ir;
    ir_memorymap_t* mm;
};
_MAKE_LIST_FOR_TYPE(ir_function)

#define _IR_OPCODE_ZERO_ARG(V) V(NOP)
#define _IR_OPCODE_ONE_ARG(V)                                                  \
    V(RET)                                                                     \
    V(JMP)                                                                     \
    V(PRINT)
#define _IR_OPCODE_TWO_ARG(V)                                                  \
    V(COPY)                                                                    \
    V(CJMP)
#define _IR_OPCODE_THREE_ARG(V)                                                \
    V(ADD)                                                                     \
    V(SUB)                                                                     \
    V(CMP)
#define _IR_OPCODE_MANY_ARG(V) V(CALL)
#define _IR_OPCODES(V)                                                         \
    _IR_OPCODE_ZERO_ARG(V)                                                     \
    _IR_OPCODE_ONE_ARG(V)                                                      \
    _IR_OPCODE_TWO_ARG(V)                                                      \
    _IR_OPCODE_THREE_ARG(V)                                                    \
    _IR_OPCODE_MANY_ARG(V)

// ir_PHI, implement this later when we add ssa
// ir_LOAD, implement mem ops later when we distiguish between mem and reg
// ir_STORE,

// only define CMP (equality, 0 or 1) and CJMP (jump if zero)
// all other operation can be implemented using this

// CALL takes the result first, then the function, followed by all parameters

typedef enum {
#define IR_ENUM(_NAME) ir_##_NAME,
    _IR_OPCODES(IR_ENUM)
#undef IR_ENUM
} ir_opcode_t;
char* getIROpcodeString(ir_opcode_t op);

typedef unsigned long vregister_t;
typedef enum {
    ir_VIRTUAL_REGISTER,
    ir_IMMEDIATE,
    ir_JMP_TARGET,
    ir_CALL_TARGET,
} ir_operand_type_t;

struct ir_operand {
    ir_operand_type_t type;
    union {
        ir_register_t* vregister;
        long immediate;
        ir_instruction_t* target;
        ir_function_t* function;
    };
};
ir_operand_t* ir_build_operand_vregister(ir_register_t* vregister);
ir_operand_t* ir_build_operand_immediate(long immediate);
ir_operand_t* ir_build_operand_target(ir_instruction_t* target);
ir_operand_t* ir_build_operand_call_temp(char* name);

struct ir_instruction {
    ir_opcode_t opcode;
    ir_operand_t** operands; // NULL terminated
    int isJmpTrgt;
    ir_instruction_t* next;
    ir_instruction_t* prev;
};
ir_instruction_t* ir_build_instruction(ir_opcode_t opcode, ...);
ir_instruction_t*
ir_build_instruction_with_operands(ir_opcode_t opcode, ir_operand_t** operands);
ir_operand_t** ir_build_operands(unsigned int noperands, ...);
ir_operand_t** ir_alloc_operands(unsigned int noperands);
// destination is always first operand

struct ir_register {
    symbol_t* symbol;
    vregister_t reg;
};
_MAKE_LIST_FOR_TYPE(ir_register)

struct ir_memorymap {
    vregister_t next_reg;
    ir_register_list_t* registers;
    ir_register_t* return_reg;
};

ir_memorymap_t* ir_init_mm();
// allocate for a symbol
ir_register_t* ir_get_irreg(ir_memorymap_t* mm, symbol_t* sym);
// allocate a temporary
ir_register_t* ir_get_temp_irreg(ir_memorymap_t* mm);

#endif
