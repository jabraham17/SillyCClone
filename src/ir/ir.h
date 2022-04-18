#ifndef _IR_IR_H_
#define _IR_IR_H_

#define _PREDECLARE_STRUCT_TYPE(_name) typedef struct _name _name##_t;
_PREDECLARE_STRUCT_TYPE(ir_function)
_PREDECLARE_STRUCT_TYPE(ir_function_list)
_PREDECLARE_STRUCT_TYPE(ir_operand)
_PREDECLARE_STRUCT_TYPE(ir_instruction)
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
};
_MAKE_LIST_FOR_TYPE(ir_function)


typedef enum {
    ir_NOP,
    ir_PHI,
    ir_LOAD,
    ir_STORE,
    ir_ADD,
    ir_SUB,
    ir_CJMP,
    ir_JMP,
} ir_opcode_t;

typedef unsigned long address_t;
typedef enum {
    ir_ADDRESS,
    ir_JMP_TARGET,
} ir_operand_type_t;

struct ir_operand {
    ir_operand_type_t type;
    union {
        address_t address;
        ir_instruction_t* target;
    };
};
struct ir_instruction {
    ir_opcode_t opcode;
    ir_operand_t* operands; // NULL terminated
    ir_instruction_t* next;
    ir_instruction_t* prev;
};


#endif
