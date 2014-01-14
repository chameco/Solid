#ifndef SOLID_VM_H
#define SOLID_VM_H

typedef struct solid_vm solid_vm;

#include "object.h"
#include <cuttle/utils.h>

typedef enum solid_ins {
	OP_END,
	OP_NOP,
	OP_PUSH,
	OP_POP,
	OP_GET,
	OP_SET,
	OP_STOREINT,
	OP_STORESTR,
	OP_STOREBOOL,
	OP_STORELIST,
	OP_PUSHLIST,
	OP_POPLIST,
	OP_MOV,
	OP_GLOBALNS,
	OP_LOCALNS,
	OP_FN,
	OP_NS,
	OP_ENDNS, 
	OP_JMP,
	OP_JMPIF,
	OP_CALL,
	OP_NOT,
} solid_ins;

typedef struct solid_bytecode {
	solid_ins ins;
	int a;
	int b;
	void *meta;
} solid_bytecode;

typedef struct solid_function {
	solid_bytecode *bcode;
	solid_object *closure;
} solid_function;

struct solid_vm {
	list_node *stack;
	list_node *all_objects;
	solid_object *regs[256];
	solid_object *namespace_stack[256];
	int namespace_stack_pointer;
};

solid_vm *solid_make_vm();

void solid_push_stack(solid_vm *vm, solid_object *o);
solid_object *solid_pop_stack(solid_vm *vm);

void solid_push_list(solid_object *list, solid_object *o);
solid_object *solid_pop_list(solid_object *list);

void solid_push_namespace(solid_vm *vm);
void solid_pop_namespace(solid_vm *vm);

void solid_push_predefined_namespace(solid_vm *vm, solid_object *namespace);
solid_object *solid_pop_predefined_namespace(solid_vm *vm);

solid_object *solid_get_current_namespace(solid_vm *vm);

void solid_gc_add_object(solid_vm *vm, solid_object *o);
void solid_gc(solid_vm *vm);

solid_object *solid_define_function(solid_vm *vm, solid_bytecode *inslist, solid_object *closure);
solid_object *solid_define_c_function(solid_vm *vm, void (*function)(solid_vm *vm));

void solid_nth_list(solid_vm *vm);
void solid_print(solid_vm *vm);
void solid_clone(solid_vm *vm);
void solid_cons(solid_vm *vm);
void solid_add(solid_vm *vm);
void solid_sub(solid_vm *vm);
void solid_mul(solid_vm *vm);
void solid_div(solid_vm *vm);
void solid_eq(solid_vm *vm);
void solid_lt(solid_vm *vm);
void solid_lte(solid_vm *vm);
void solid_gt(solid_vm *vm);
void solid_gte(solid_vm *vm);

solid_object *solid_not(solid_vm *vm, solid_object *o);

solid_bytecode solid_bc(solid_ins i, int a, int b, void *meta);

void solid_call_func(solid_vm *vm, solid_object *func);

#endif
