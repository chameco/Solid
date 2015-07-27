#include "vm.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "utils.h"
#include "object.h"

solid_vm *solid_make_vm()
{
	solid_vm *ret = (solid_vm *) malloc(sizeof(solid_vm));
	ret->stack = make_list();
	ret->all_objects = make_list();
	memset(ret->regs, 0, 256);
	memset(ret->namespace_stack, 0, 256);
	ret->namespace_stack[0] = solid_instance(ret);
	ret->namespace_stack_pointer = 0;
	solid_set_namespace(solid_get_current_namespace(ret), solid_str(ret, "!!"), solid_define_c_function(ret, solid_nth_list));
	solid_set_namespace(solid_get_current_namespace(ret), solid_str(ret, "print"), solid_define_c_function(ret, solid_print));
	solid_set_namespace(solid_get_current_namespace(ret), solid_str(ret, "clone"), solid_define_c_function(ret, solid_clone));
	solid_set_namespace(solid_get_current_namespace(ret), solid_str(ret, ":"), solid_define_c_function(ret, solid_cons));
	solid_set_namespace(solid_get_current_namespace(ret), solid_str(ret, "+"), solid_define_c_function(ret, solid_add));
	solid_set_namespace(solid_get_current_namespace(ret), solid_str(ret, "-"), solid_define_c_function(ret, solid_sub));
	solid_set_namespace(solid_get_current_namespace(ret), solid_str(ret, "*"), solid_define_c_function(ret, solid_mul));
	solid_set_namespace(solid_get_current_namespace(ret), solid_str(ret, "/"), solid_define_c_function(ret, solid_div));
	solid_set_namespace(solid_get_current_namespace(ret), solid_str(ret, "=="), solid_define_c_function(ret, solid_eq));
	solid_set_namespace(solid_get_current_namespace(ret), solid_str(ret, "<"), solid_define_c_function(ret, solid_lt));
	solid_set_namespace(solid_get_current_namespace(ret), solid_str(ret, "<="), solid_define_c_function(ret, solid_lte));
	solid_set_namespace(solid_get_current_namespace(ret), solid_str(ret, ">"), solid_define_c_function(ret, solid_gt));
	solid_set_namespace(solid_get_current_namespace(ret), solid_str(ret, ">="), solid_define_c_function(ret, solid_gte));
	solid_set_namespace(solid_get_current_namespace(ret), solid_str(ret, "gc"), solid_define_c_function(ret, solid_gc));
	return ret;
}

void solid_push_stack(solid_vm *vm, solid_object *o)
{
	insert_list(vm->stack, (void *) o);
}

solid_object *solid_pop_stack(solid_vm *vm)
{
	list_node *n = vm->stack->next;
	if (n->data == NULL) {
		log_err("Stack is empty");
		exit(1);
	} else {
		n->prev->next = n->next;
		n->next->prev = n->prev;
		solid_object *ret = (solid_object *) n->data;
		free(n);
		return ret;
	}
}

void solid_push_list(solid_object *list, solid_object *o)
{
	insert_list(list->data.list, (void *) o);
}

solid_object *solid_pop_list(solid_object *list)
{
	list_node *n = ((list_node *) list)->next;
	if (n->data == NULL) {
		log_err("List is empty");
		exit(1);
	} else {
		n->prev->next = n->next;
		n->next->prev = n->prev;
		solid_object *ret = (solid_object *) n->data;
		return ret;
	}
}

void solid_push_namespace(solid_vm *vm)
{
	if (vm->namespace_stack_pointer >= 255) {
		log_err("Namespace stack overflow");
		exit(1);
	}
	vm->namespace_stack[vm->namespace_stack_pointer+1] = solid_clone_object(vm, vm->namespace_stack[vm->namespace_stack_pointer]);
	vm->namespace_stack_pointer++;
}

void solid_pop_namespace(solid_vm *vm)
{
	if (vm->namespace_stack_pointer == 0) {
		log_err("Namespace stack underflow");
		exit(1);
	}
	solid_delete_object(vm, vm->namespace_stack[vm->namespace_stack_pointer--]);
}

void solid_push_predefined_namespace(solid_vm *vm, solid_object *namespace)
{
	if (vm->namespace_stack_pointer >= 255) {
		log_err("Namespace stack overflow");
		exit(1);
	}
	vm->namespace_stack[++vm->namespace_stack_pointer] = namespace;
}

solid_object *solid_pop_predefined_namespace(solid_vm *vm)
{
	if (vm->namespace_stack_pointer == 0) {
		log_err("Namespace stack underflow");
		exit(1);
	}
	return vm->namespace_stack[vm->namespace_stack_pointer--];
}

solid_object *solid_get_current_namespace(solid_vm *vm)
{
	return vm->namespace_stack[vm->namespace_stack_pointer];
}

void solid_gc_add_object(solid_vm *vm, solid_object *o)
{
	insert_list(vm->all_objects, (void *) o);
}

void solid_gc(solid_vm *vm)
{
	for (int i = vm->namespace_stack_pointer; i >= 0; i--) {
		solid_mark_object(vm->namespace_stack[i], 1);
	}

	list_node *c;
	solid_object *cur;
	for (c = vm->all_objects->next; c != NULL; c = c->next) {
		if (c->data != NULL) {
			cur = (solid_object *) c->data;
			if (cur->marked == 0) {
				solid_delete_object(vm, cur);
				list_node *prev = c->prev;
				c->prev->next = c->next;
				c->next->prev = c->prev;
				free(c);
				c = prev;
			} else if (cur->marked == 1) {
				cur->marked = 0;
			}
		}
	}
	vm->regs[255] = solid_str(vm, "Trash compactor completing operation");
}

solid_object *solid_define_function(solid_vm *vm, solid_bytecode *inslist)
{
	solid_object *ret = solid_func(vm);
	solid_function *fval = (solid_function *) malloc(sizeof(solid_function));
	fval->bcode = inslist;
	ret->data.func = fval;
	return ret;
}

solid_object *solid_define_c_function(solid_vm *vm, void (*function)(solid_vm *vm))
{
	solid_object *ret = solid_cfunc(vm);
	ret->data.cfunc = function;
	return ret;
}

void solid_nth_list(solid_vm *vm)
{
	solid_object *index = solid_pop_stack(vm);
	int i = solid_get_int_value(index);
	solid_object *list = solid_pop_stack(vm);
	list_node *l = list->data.list;
	int counter = 0;
	list_node *c;
	for (c = l->next; c->next != NULL; c = c->next) {
		if (c->data != NULL) {
			if (counter == i) {
				vm->regs[255] = (solid_object *) c->data;
				return;
			}
			counter++;
		}
	}
	log_err("List index out of bounds");
	exit(1);
}

void solid_print(solid_vm *vm)
{
	solid_object *in = solid_pop_stack(vm);
	if (in->type == T_INT) {
		fprintf(stdout, "%d\n", solid_get_int_value(in));
	} else if (in->type == T_DOUBLE) {
		fprintf(stdout, "%lf\n", solid_get_double_value(in));
	} else if (in->type == T_STR) {
		fprintf(stdout, "%s\n", solid_get_str_value(in));
	} else if (in->type == T_FUNC) {
		fprintf(stdout, "%s\n", "Function");
	} else if (in->type == T_INSTANCE) {
		fprintf(stdout, "%s\n", "Object");
	} else {
		fprintf(stdout, "Unknown of type %d\n", in->type);
	}
	vm->regs[255] = in;
}

void solid_clone(solid_vm *vm)
{
	solid_object *ns = solid_pop_stack(vm);
	vm->regs[255] = solid_clone_object(vm, ns);
}

void solid_cons(solid_vm *vm)
{
	solid_object *list = solid_pop_stack(vm);
	solid_object *elem = solid_pop_stack(vm);
	list_node *ret = make_list();
	insert_list(ret, (void *) elem);
	ret->next->next = list->data.list;
	list->data.list->prev = ret->next;
	vm->regs[255] = solid_list(vm, ret);
}

void solid_add(solid_vm *vm)
{
	solid_object *b = solid_pop_stack(vm);
	solid_object *a = solid_pop_stack(vm);
	if ((a->type != T_INT && a->type != T_DOUBLE) || (b->type != T_INT && b->type != T_DOUBLE)) {
		log_err("Attempt to apply operator \"+\" on invalid types");
		exit(1);
	}
	if (a->type == T_DOUBLE || b->type == T_DOUBLE) {
		vm->regs[255] = solid_double(vm, solid_get_double_value(a) + solid_get_double_value(b));
	} else {
		vm->regs[255] = solid_int(vm, solid_get_int_value(a) + solid_get_int_value(b));
	}
}

void solid_sub(solid_vm *vm)
{
	solid_object *b = solid_pop_stack(vm);
	solid_object *a = solid_pop_stack(vm);
	if ((a->type != T_INT && a->type != T_DOUBLE) || (b->type != T_INT && b->type != T_DOUBLE)) {
		log_err("Attempt to apply operator \"-\" on invalid types");
		exit(1);
	}
	if (a->type == T_DOUBLE || b->type == T_DOUBLE) {
		vm->regs[255] = solid_double(vm, solid_get_double_value(a) - solid_get_double_value(b));
	} else {
		vm->regs[255] = solid_int(vm, solid_get_int_value(a) - solid_get_int_value(b));
	}
}

void solid_mul(solid_vm *vm)
{
	solid_object *b = solid_pop_stack(vm);
	solid_object *a = solid_pop_stack(vm);
	if ((a->type != T_INT && a->type != T_DOUBLE) || (b->type != T_INT && b->type != T_DOUBLE)) {
		log_err("Attempt to apply operator \"*\" on invalid types");
		exit(1);
	}
	if (a->type == T_DOUBLE || b->type == T_DOUBLE) {
		vm->regs[255] = solid_double(vm, solid_get_double_value(a) * solid_get_double_value(b));
	} else {
		vm->regs[255] = solid_int(vm, solid_get_int_value(a) * solid_get_int_value(b));
	}
}

void solid_div(solid_vm *vm)
{
	solid_object *b = solid_pop_stack(vm);
	solid_object *a = solid_pop_stack(vm);
	if ((a->type != T_INT && a->type != T_DOUBLE) || (b->type != T_INT && b->type != T_DOUBLE)) {
		log_err("Attempt to apply operator \"\\\" on invalid types");
		exit(1);
	}
	if (a->type == T_DOUBLE || b->type == T_DOUBLE) {
		vm->regs[255] = solid_double(vm, solid_get_double_value(a) / solid_get_double_value(b));
	} else {
		vm->regs[255] = solid_int(vm, solid_get_int_value(a) / solid_get_int_value(b));
	}
}

void solid_eq(solid_vm *vm)
{
	solid_object *b = solid_pop_stack(vm);
	solid_object *a = solid_pop_stack(vm);
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \"==\" on invalid types");
		exit(1);
	}
	vm->regs[255] = solid_bool(vm, solid_get_int_value(a) == solid_get_int_value(b));
}

void solid_lt(solid_vm *vm)
{
	solid_object *b = solid_pop_stack(vm);
	solid_object *a = solid_pop_stack(vm);
	if ((a->type != T_INT && a->type != T_DOUBLE) || (b->type != T_INT && b->type != T_DOUBLE)) {
		log_err("Attempt to apply operator \"<\" on invalid types");
		exit(1);
	}
	if (a->type == T_DOUBLE || b->type == T_DOUBLE) {
		vm->regs[255] = solid_bool(vm, solid_get_double_value(a) < solid_get_double_value(b));
	} else {
		vm->regs[255] = solid_bool(vm, solid_get_int_value(a) < solid_get_int_value(b));
	}
}

void solid_lte(solid_vm *vm)
{
	solid_object *b = solid_pop_stack(vm);
	solid_object *a = solid_pop_stack(vm);
	if ((a->type != T_INT && a->type != T_DOUBLE) || (b->type != T_INT && b->type != T_DOUBLE)) {
		log_err("Attempt to apply operator \"<=\" on invalid types");
		exit(1);
	}
	if (a->type == T_DOUBLE || b->type == T_DOUBLE) {
		vm->regs[255] = solid_bool(vm, solid_get_double_value(a) <= solid_get_double_value(b));
	} else {
		vm->regs[255] = solid_bool(vm, solid_get_int_value(a) <= solid_get_int_value(b));
	}
}

void solid_gt(solid_vm *vm)
{
	solid_object *b = solid_pop_stack(vm);
	solid_object *a = solid_pop_stack(vm);
	if ((a->type != T_INT && a->type != T_DOUBLE) || (b->type != T_INT && b->type != T_DOUBLE)) {
		log_err("Attempt to apply operator \">\" on invalid types");
		exit(1);
	}
	if (a->type == T_DOUBLE || b->type == T_DOUBLE) {
		vm->regs[255] = solid_bool(vm, solid_get_double_value(a) > solid_get_double_value(b));
	} else {
		vm->regs[255] = solid_bool(vm, solid_get_int_value(a) > solid_get_int_value(b));
	}
}

void solid_gte(solid_vm *vm)
{
	solid_object *b = solid_pop_stack(vm);
	solid_object *a = solid_pop_stack(vm);
	if ((a->type != T_INT && a->type != T_DOUBLE) || (b->type != T_INT && b->type != T_DOUBLE)) {
		log_err("Attempt to apply operator \">=\" on invalid types");
		exit(1);
	}
	if (a->type == T_DOUBLE || b->type == T_DOUBLE) {
		vm->regs[255] = solid_bool(vm, solid_get_double_value(a) >= solid_get_double_value(b));
	} else {
		vm->regs[255] = solid_bool(vm, solid_get_int_value(a) >= solid_get_int_value(b));
	}
}

solid_object *solid_not(solid_vm *vm, solid_object *o)
{
	if (o->type != T_BOOL && o->type != T_INT) {
		log_err("Attempt to negate invalid type");
		exit(1);
	}
	return solid_bool(vm, !solid_get_bool_value(o));
}

solid_bytecode solid_bc(solid_ins i, int a, int b, void *meta)
{
	solid_bytecode ret;
	ret.ins = i;
	ret.a = a;
	ret.b = b;
	ret.meta = meta;
	return ret;
}

void solid_call_func(solid_vm *vm, solid_object *func)
{
	if (func->type == T_FUNC) {
		solid_push_namespace(vm);
		solid_set_namespace(solid_get_current_namespace(vm), solid_str(vm, "$"), func);
		solid_bytecode *inslist = ((solid_function *) func->data.func)->bcode;
		solid_bytecode cur;
		int pos;
		for (pos = 0, cur = inslist[pos]; cur.ins != OP_END; cur = inslist[++pos]) {
			//debug("ins: %d, stack height: %d", cur.ins, length_list(vm->stack));
			switch(cur.ins) {
				case OP_END:
					return;
					break;
				case OP_NOP:
					break;
				case OP_PUSH:
					solid_push_stack(vm, vm->regs[cur.a]);
					break;
				case OP_POP:
					vm->regs[cur.a] = solid_pop_stack(vm);
					break;
				case OP_GET:
					vm->regs[cur.a] = solid_get_namespace(vm->regs[cur.b], vm->regs[cur.a]);
					break;
				case OP_SET:
					solid_set_namespace(vm->regs[cur.b], solid_str(vm, (char *) cur.meta), vm->regs[cur.a]);
					break;
				case OP_STOREINT:
					vm->regs[cur.a] = solid_int(vm, cur.b);
					break;
				case OP_STOREDOUBLE:
					vm->regs[cur.a] = solid_double(vm, *((double *) cur.meta));
					break;
				case OP_STORESTR:
					vm->regs[cur.a] = solid_str(vm, (char *) cur.meta);
					break;
				case OP_STOREBOOL:
					vm->regs[cur.a] = solid_bool(vm, cur.b);
					break;
				case OP_STORELIST:
					vm->regs[cur.a] = solid_list(vm, make_list());
					break;
				case OP_PUSHLIST:
					solid_push_list(vm->regs[cur.a], vm->regs[cur.b]);
					break;
				case OP_POPLIST:
					vm->regs[cur.a] = solid_pop_list(vm->regs[cur.b]);
					break;
				case OP_MOV:
					vm->regs[cur.a] = vm->regs[cur.b];
					break;
				case OP_GLOBALNS:
					vm->regs[cur.a] = vm->namespace_stack[0];
					break;
				case OP_LOCALNS:
					vm->regs[cur.a] = solid_get_current_namespace(vm);
					break;
				case OP_FN:
					vm->regs[cur.a] = solid_define_function(vm, (solid_bytecode *) cur.meta);
					break;
				case OP_NS:
					vm->regs[cur.a] = solid_clone_object(vm, solid_get_current_namespace(vm));
					solid_push_predefined_namespace(vm, vm->regs[cur.a]);
					break;
				case OP_ENDNS:
					vm->regs[cur.a] = solid_pop_predefined_namespace(vm);
					break;
				case OP_JMP:
					pos = cur.a - 1;
					break;
				case OP_JMPIF:
					if (solid_get_bool_value(vm->regs[cur.b])) {
						pos = cur.a - 1;
					}
					break;
				case OP_CALL:
					solid_call_func(vm, vm->regs[cur.a]);
					break;
				case OP_NOT:
					vm->regs[cur.a] = solid_not(vm, vm->regs[cur.a]);
					break;
			}
		}
		solid_pop_namespace(vm);
	} else if (func->type == T_CFUNC) {
		((void (*)(solid_vm *))(func->data.func))(vm);
	} else {
		debug("func->type: %d", func->type);
		log_err("Object not a function");
		exit(1);
	}
}
