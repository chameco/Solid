#include "vm.h"

solid_vm *make_solid_vm()
{
	solid_vm *ret = (solid_vm *) malloc(sizeof(solid_vm));
	ret->stack = make_list();
	memset(ret->regs, 0, 256);
	memset(ret->namespace_stack, 0, 256);
	ret->namespace_stack[0] = solid_instance();
	ret->namespace_stack_pointer = 0;
	set_namespace(get_current_namespace(ret), solid_str("!!"), define_c_function(solid_nth_list));
	set_namespace(get_current_namespace(ret), solid_str("print"), define_c_function(solid_print));
	set_namespace(get_current_namespace(ret), solid_str("+"), define_c_function(solid_add));
	set_namespace(get_current_namespace(ret), solid_str("-"), define_c_function(solid_sub));
	set_namespace(get_current_namespace(ret), solid_str("*"), define_c_function(solid_mul));
	set_namespace(get_current_namespace(ret), solid_str("/"), define_c_function(solid_div));
	set_namespace(get_current_namespace(ret), solid_str("=="), define_c_function(solid_eq));
	set_namespace(get_current_namespace(ret), solid_str("<"), define_c_function(solid_lt));
	set_namespace(get_current_namespace(ret), solid_str("<="), define_c_function(solid_lte));
	set_namespace(get_current_namespace(ret), solid_str(">"), define_c_function(solid_gt));
	set_namespace(get_current_namespace(ret), solid_str(">="), define_c_function(solid_gte));
	return ret;
}

void push_stack(solid_vm *vm, solid_object *o)
{
	//debug("pushed a %d", o->type);
	insert_list(vm->stack, (void *) o);
}

solid_object *pop_stack(solid_vm *vm)
{
	list_node *n = vm->stack->next;
	if (n->data == NULL) {
		log_err("Stack is empty");
		exit(1);
	} else {
		n->prev->next = n->next;
		n->next->prev = n->prev;
		solid_object *ret = (solid_object *) n->data;
		//debug("popped a %d", ret->type);
		return ret;
	}
}

void push_list(solid_object *list, solid_object *o)
{
	insert_list((list_node *) list->data, (void *) o);
}

solid_object *pop_list(solid_object *list)
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

void push_namespace(solid_vm *vm)
{
	vm->namespace_stack[vm->namespace_stack_pointer+1] = clone_object(vm->namespace_stack[vm->namespace_stack_pointer]);
	vm->namespace_stack_pointer++;
}

void pop_namespace(solid_vm *vm)
{
	free(vm->namespace_stack[vm->namespace_stack_pointer--]);
}

void push_predefined_namespace(solid_vm *vm, solid_object *namespace)
{
	vm->namespace_stack[++vm->namespace_stack_pointer] = namespace;
}

solid_object *pop_predefined_namespace(solid_vm *vm)
{
	return vm->namespace_stack[vm->namespace_stack_pointer--];
}

solid_object *get_current_namespace(solid_vm *vm)
{
	return vm->namespace_stack[vm->namespace_stack_pointer];
}

solid_object *define_function(solid_bytecode *inslist, solid_object *closure)
{
	solid_object *ret = solid_func();
	solid_function *fval = (solid_function *) malloc(sizeof(solid_function));
	fval->bcode = inslist;
	fval->closure = closure;
	ret->data = (void *) fval;
	return ret;
}

solid_object *define_c_function(void (*function)(solid_vm *vm))
{
	solid_object *ret = solid_cfunc();
	ret->data = (void *) function;
	return ret;
}

void solid_nth_list(solid_vm *vm)
{
	solid_object *index = pop_stack(vm);
	int i = get_int_value(index);
	solid_object *list = pop_stack(vm);
	list_node *l = (list_node *) list->data;
	int counter = 0;
	list_node *c;
	for (c = l; c->next != NULL; c = c->next) {
		if (c->data != NULL) {
			if (counter - 1 == i) {
				vm->regs[255] = (solid_object *) c->data;
				return;
			}
		}
		counter++;
	}
	log_err("List index out of bounds");
	exit(1);
}

void solid_print(solid_vm *vm)
{
	solid_object *in = pop_stack(vm);
	if (in->type == T_INT) {
		fprintf(stdout, "%d\n", get_int_value(in));
	} else if (in->type == T_STR) {
		fprintf(stdout, "%s\n", get_str_value(in));
	} else if (in->type == T_FUNC) {
		fprintf(stdout, "%s\n", "Function");
	} else if (in->type == T_INSTANCE) {
		fprintf(stdout, "%s\n", "Object");
	} else {
		fprintf(stdout, "Unknown of type %d\n", in->type);
	}
	vm->regs[255] = in;
}

void solid_add(solid_vm *vm)
{
	solid_object *b = pop_stack(vm);
	solid_object *a = pop_stack(vm);
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \"+\" on invalid types");
		exit(1);
	}
	vm->regs[255] = solid_int(get_int_value(a) + get_int_value(b));
}

void solid_sub(solid_vm *vm)
{
	solid_object *b = pop_stack(vm);
	solid_object *a = pop_stack(vm);
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \"-\" on invalid types");
		exit(1);
	}
	vm->regs[255] = solid_int(get_int_value(a) - get_int_value(b));
}

void solid_mul(solid_vm *vm)
{
	solid_object *b = pop_stack(vm);
	solid_object *a = pop_stack(vm);
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \"*\" on invalid types");
		exit(1);
	}
	vm->regs[255] = solid_int(get_int_value(a) * get_int_value(b));
}

void solid_div(solid_vm *vm)
{
	solid_object *b = pop_stack(vm);
	solid_object *a = pop_stack(vm);
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \"\\\" on invalid types");
		exit(1);
	}
	vm->regs[255] = solid_int(get_int_value(a) / get_int_value(b));
}

void solid_eq(solid_vm *vm)
{
	solid_object *b = pop_stack(vm);
	solid_object *a = pop_stack(vm);
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \"==\" on invalid types");
		exit(1);
	}
	vm->regs[255] = solid_bool(get_int_value(a) == get_int_value(b));
}

void solid_lt(solid_vm *vm)
{
	solid_object *b = pop_stack(vm);
	solid_object *a = pop_stack(vm);
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \"<\" on invalid types");
		exit(1);
	}
	vm->regs[255] = solid_bool(get_int_value(a) < get_int_value(b));
}

void solid_lte(solid_vm *vm)
{
	solid_object *b = pop_stack(vm);
	solid_object *a = pop_stack(vm);
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \"<=\" on invalid types");
		exit(1);
	}
	vm->regs[255] = solid_bool(get_int_value(a) <= get_int_value(b));
}

void solid_gt(solid_vm *vm)
{
	solid_object *b = pop_stack(vm);
	solid_object *a = pop_stack(vm);
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \">\" on invalid types");
		exit(1);
	}
	vm->regs[255] = solid_bool(get_int_value(a) > get_int_value(b));
}

void solid_gte(solid_vm *vm)
{
	solid_object *b = pop_stack(vm);
	solid_object *a = pop_stack(vm);
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \">=\" on invalid types");
		exit(1);
	}
	vm->regs[255] = solid_bool(get_int_value(a) >= get_int_value(b));
}

solid_object *solid_not(solid_object *o)
{
	if (o->type != T_BOOL && o->type != T_INT) {
		debug("o->type: %d, strval: %s", o->type, get_str_value(o));
		log_err("Attempt to negate invalid type");
		exit(1);
	}
	return solid_bool(!get_bool_value(o));
}

solid_bytecode bc(solid_ins i, int a, int b, void *meta)
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
		if (((solid_function *) func->data)->closure != NULL) {
			push_predefined_namespace(vm, ((solid_function *) func->data)->closure);
		}
		solid_bytecode *inslist = ((solid_function *) func->data)->bcode;
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
					push_stack(vm, vm->regs[cur.a]);
					break;
				case OP_POP:
					vm->regs[cur.a] = pop_stack(vm);
					regdebug("vm->regs[%d]->type: %d", cur.a, vm->regs[cur.a]->type);
					break;
				case OP_GET:
					vm->regs[cur.a] = get_namespace(vm->regs[cur.b], vm->regs[cur.a]);
					regdebug("vm->regs[%d]->type: %d", cur.a, vm->regs[cur.a]->type);
					break;
				case OP_SET:
					set_namespace(vm->regs[cur.b], solid_str((char *) cur.meta), vm->regs[cur.a]);
					break;
				case OP_STOREINT:
					vm->regs[cur.a] = solid_int(cur.b);
					regdebug("vm->regs[%d]->type: %d", cur.a, vm->regs[cur.a]->type);
					break;
				case OP_STORESTR:
					vm->regs[cur.a] = solid_str((char *) cur.meta);
					regdebug("vm->regs[%d]->type: %d", cur.a, vm->regs[cur.a]->type);
					break;
				case OP_STOREBOOL:
					vm->regs[cur.a] = solid_bool(cur.b);
					regdebug("vm->regs[%d]->type: %d", cur.a, vm->regs[cur.a]->type);
					break;
				case OP_STORELIST:
					vm->regs[cur.a] = solid_list(make_list());
					regdebug("vm->regs[%d]->type: %d", cur.a, vm->regs[cur.a]->type);
					break;
				case OP_PUSHLIST:
					push_list(vm->regs[cur.a], vm->regs[cur.b]);
					break;
				case OP_POPLIST:
					vm->regs[cur.a] = pop_list(vm->regs[cur.b]);
					break;
				case OP_MOV:
					vm->regs[cur.a] = vm->regs[cur.b];
					regdebug("vm->regs[%d]->type: %d", cur.a, vm->regs[cur.a]->type);
					break;
				case OP_GLOBALNS:
					vm->regs[cur.a] = vm->namespace_stack[0];
					regdebug("vm->regs[%d]->type: %d", cur.a, vm->regs[cur.a]->type);
					break;
				case OP_LOCALNS:
					vm->regs[cur.a] = get_current_namespace(vm);
					regdebug("vm->regs[%d]->type: %d", cur.a, vm->regs[cur.a]->type);
					break;
				case OP_FN:
					vm->regs[cur.a] = define_function((solid_bytecode *) cur.meta, get_current_namespace(vm));
					regdebug("vm->regs[%d]->type: %d", cur.a, vm->regs[cur.a]->type);
					break;
				case OP_NS:
					vm->regs[cur.a] = clone_object(get_current_namespace(vm));
					push_predefined_namespace(vm, vm->regs[cur.a]);
					break;
				case OP_ENDNS:
					vm->regs[cur.a] = pop_predefined_namespace(vm);
					break;
				case OP_JMP:
					pos = cur.a;
					break;
				case OP_JMPIF:
					if (get_bool_value(vm->regs[cur.b])) {
						pos = cur.a;
					}
					break;
				case OP_CALL:
					solid_call_func(vm, vm->regs[cur.a]);
					break;
				case OP_NOT:
					vm->regs[cur.a] = solid_not(vm->regs[cur.a]);
					regdebug("vm->regs[%d]->type: %d", cur.a, vm->regs[cur.a]->type);
					break;
			}
		}
		if (((solid_function *) func->data)->closure != NULL) {
			pop_predefined_namespace(vm);
		}
	} else if (func->type == T_CFUNC) {
		((void (*)(solid_vm *))(func->data))(vm);
	} else {
		debug("func->type: %d", func->type);
		log_err("Object not a function");
		exit(1);
	}
}
