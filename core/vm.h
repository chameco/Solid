#ifndef VM_H
#define VM_H

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "debug.h"

#include "utils.h"
#include "object.h"

typedef enum solid_ins {
	OP_END,
	OP_PUSH,
	OP_POP,
	OP_GET,
	OP_SET,
	OP_PUSHINT,
	OP_PUSHSTR,
	OP_GLOBALNS,
	OP_DEF,
	OP_CLASS,
	OP_JMP,
	OP_JMPIF,
	OP_CALL,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_EQ,
	OP_NOT,
	OP_LT,
	OP_LTE,
	OP_GT,
	OP_GTE,
} solid_ins;

typedef struct solid_bytecode {
	solid_ins ins;
	int a;
	int b;
	char *meta;
} solid_bytecode;

typedef struct solid_vm {
	list_node *stack;
	solid_object *global_ns;
	solid_object *regs[256];
} solid_vm;

solid_vm *make_solid_vm();

void push_stack(solid_vm *vm, solid_object *o);

solid_object *pop_stack(solid_vm *vm);

solid_object *define_function(solid_bytecode *inslist);

void define_class(solid_object *name);

solid_object *solid_add(solid_object *a, solid_object *b);

solid_object *solid_sub(solid_object *a, solid_object *b);

solid_object *solid_mul(solid_object *a, solid_object *b);

solid_object *solid_div(solid_object *a, solid_object *b);

solid_object *solid_eq(solid_object *a, solid_object *b);

solid_object *solid_not(solid_object *o);

solid_object *solid_lt(solid_object *a, solid_object *b);

solid_object *solid_lte(solid_object *a, solid_object *b);

solid_object *solid_gt(solid_object *a, solid_object *b);

solid_object *solid_gte(solid_object *a, solid_object *b);

solid_object *solid_eval_bytecode(solid_vm *vm, solid_object *func);

solid_object *call_method(solid_vm *vm, solid_object *o, solid_object *method);

#endif
