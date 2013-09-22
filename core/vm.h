#ifndef SOLID_VM_H
#define SOLID_VM_H

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <cuttle/utils.h>
#include <cuttle/debug.h>

#include "object.h"

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

typedef struct solid_vm {
	list_node *stack;
	solid_object *regs[256];
	solid_object *namespace_stack[256];
	int namespace_stack_pointer;
} solid_vm;

solid_vm *make_solid_vm();

void push_stack(solid_vm *vm, solid_object *o);
solid_object *pop_stack(solid_vm *vm);

void push_list(solid_object *list, solid_object *o);
solid_object *pop_list(solid_object *list);

void push_namespace(solid_vm *vm);
void pop_namespace(solid_vm *vm);

void push_predefined_namespace(solid_vm *vm, solid_object *namespace);
solid_object *pop_predefined_namespace(solid_vm *vm);

solid_object *get_current_namespace(solid_vm *vm);

solid_object *define_function(solid_bytecode *inslist, solid_object *closure);
solid_object *define_c_function(void (*function)(solid_vm *vm));

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

solid_object *solid_not(solid_object *o);

solid_bytecode bc(solid_ins i, int a, int b, void *meta);

void solid_call_func(solid_vm *vm, solid_object *func);

#endif
