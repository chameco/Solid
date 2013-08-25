#include "vm.h"

solid_vm *make_solid_vm()
{
	solid_vm *ret = (solid_vm *) malloc(sizeof(solid_vm));
	ret->stack = make_list();
	memset(ret->regs, 0, 256);
	memset(ret->call_stack, 0, 256);
	ret->call_stack_pointer = 0;
	ret->call_stack_pointer = 0;
	memset(ret->namespace_stack, 0, 256);
	ret->namespace_stack[0] = solid_instance();
	ret->namespace_stack_pointer = 0;
	initialize_builtin_classes(ret->namespace_stack[0]);
	return ret;
}

void push_stack(solid_vm *vm, solid_object *o)
{
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
		return ret;
	}
}

void push_namespace(solid_vm *vm)
{
	vm->namespace_stack[++vm->namespace_stack_pointer] = solid_instance();
}

void pop_namespace(solid_vm *vm)
{
	free(vm->namespace_stack[vm->namespace_stack_pointer--]);
}

void push_predefined_namespace(solid_vm *vm, solid_object *namespace)
{
	vm->namespace_stack[++vm->namespace_stack_pointer] = namespace;
}

void pop_predefined_namespace(solid_vm *vm)
{
	vm->namespace_stack_pointer--;
}

solid_object *get_current_namespace(solid_vm *vm)
{
	return vm->namespace_stack[vm->namespace_stack_pointer];
}

solid_object *define_function(solid_bytecode *inslist)
{
	solid_object *ret = solid_func();
	solid_function *fval = (solid_function *) malloc(sizeof(solid_function));
	fval->bcode = inslist;
	ret->data = (void *) fval;
	return ret;
}

solid_object *define_class(solid_object *super)
{
	if (super == NULL) {
		super = get_builtin_class_object();
	}
	solid_object *ret = solid_class(super);
	return ret;
}

solid_object *solid_add(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \"+\" on invalid types");
		exit(1);
	}
	return solid_int(get_int_value(a) + get_int_value(b));
}

solid_object *solid_sub(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \"-\" on invalid types");
		exit(1);
	}
	return solid_int(get_int_value(a) - get_int_value(b));
}

solid_object *solid_mul(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \"*\" on invalid types");
		exit(1);
	}
	return solid_int(get_int_value(a) * get_int_value(b));
}

solid_object *solid_div(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \"\\\" on invalid types");
		exit(1);
	}
	return solid_int(get_int_value(a) / get_int_value(b));
}

solid_object *solid_eq(solid_object *a,	solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \"==\" on invalid types");
		exit(1);
	}
	return solid_bool(get_int_value(a) == get_int_value(b));
}

solid_object *solid_not(solid_object *o)
{
	if (o->type != T_BOOL && o->type != T_INT) {
		log_err("Attempt to negate invalid type");
		exit(1);
	}
	return solid_bool(!get_bool_value(o));
}

solid_object *solid_lt(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \"<\" on invalid types");
		exit(1);
	}
	return solid_bool(get_int_value(a) < get_int_value(b));
}

solid_object *solid_lte(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \"<=\" on invalid types");
		exit(1);
	}
	return solid_bool(get_int_value(a) <= get_int_value(b));
}

solid_object *solid_gt(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \">\" on invalid types");
		exit(1);
	}
	return solid_bool(get_int_value(a) > get_int_value(b));
}

solid_object *solid_gte(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("Attempt to apply operator \">=\" on invalid types");
		exit(1);
	}
	return solid_bool(get_int_value(a) >= get_int_value(b));
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

solid_object *solid_eval_bytecode(solid_vm *vm, solid_object *func)
{
	if (func->type == T_FUNC) {
		solid_bytecode *inslist = ((solid_function *) func->data)->bcode;
		solid_bytecode cur;
		int pos;
		solid_object *temp;
		for (pos = 0, cur = inslist[pos]; cur.ins != OP_END; cur = inslist[++pos]) {
			switch(cur.ins) {
				case OP_NOP:
					break;
				case OP_END:
					return vm->regs[0];
					break;
				case OP_PUSH:
					push_stack(vm, vm->regs[cur.a]);
					break;
				case OP_POP:
					vm->regs[cur.a] = pop_stack(vm);
					break;
				case OP_GET:
					vm->regs[cur.b] = get_namespace(vm->regs[cur.a], vm->regs[cur.b]);
					break;
				case OP_SET:
					set_namespace(vm->regs[cur.a], solid_str((char *) cur.meta), vm->regs[cur.b]);
					break;
				case OP_PUSHINT:
					push_stack(vm, solid_int(cur.a));
					break;
				case OP_PUSHSTR:
					push_stack(vm, solid_str((char *) cur.meta));
					break;
				case OP_PUSHBOOL:
					push_stack(vm, solid_bool(cur.a));
					break;
				case OP_GLOBALNS:
					vm->regs[cur.a] = vm->namespace_stack[0];
					break;
				case OP_LOCALNS:
					vm->regs[cur.a] = get_current_namespace(vm);
					break;
				case OP_FN:
					push_stack(vm, define_function((solid_bytecode *) cur.meta));
					break;
				case OP_CLASS:
					if (vm->regs[cur.a]->type == T_INT) {
						temp = define_class(NULL);
					} else {
						temp = define_class(vm->regs[cur.a]);
					}
					push_stack(vm, temp);
					push_predefined_namespace(vm, temp);
					break;
				case OP_ENDCLASS:
					pop_predefined_namespace(vm);
					break;
				case OP_NEW:
					push_stack(vm, instance_from_class(vm->regs[cur.a]));
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
					push_namespace(vm);
					vm->call_stack[++(vm->call_stack_pointer)] = pos;
					solid_eval_bytecode(vm, vm->regs[cur.a]);
					pop_namespace(vm);
					break;
				case OP_RETURN:
					pos = vm->call_stack[vm->call_stack_pointer--];
					break;
				case OP_ADD:
					vm->regs[cur.a] = solid_add(vm->regs[cur.a], vm->regs[cur.b]);
					break;
				case OP_SUB:
					vm->regs[cur.a] = solid_sub(vm->regs[cur.a], vm->regs[cur.b]);
					break;
				case OP_MUL:
					vm->regs[cur.a] = solid_mul(vm->regs[cur.a], vm->regs[cur.b]);
					break;
				case OP_DIV:
					vm->regs[cur.a] = solid_div(vm->regs[cur.a], vm->regs[cur.b]);
					break;
				case OP_EQ:
					vm->regs[cur.a] = solid_eq(vm->regs[cur.a], vm->regs[cur.b]);
					break;
				case OP_NOT:
					vm->regs[cur.a] = solid_not(vm->regs[cur.a]);
					break;
				case OP_LT:
					vm->regs[cur.a] = solid_lt(vm->regs[cur.a], vm->regs[cur.b]);
					break;
				case OP_LTE:
					vm->regs[cur.a] = solid_lte(vm->regs[cur.a], vm->regs[cur.b]);
					break;
				case OP_GT:
					vm->regs[cur.a] = solid_gt(vm->regs[cur.a], vm->regs[cur.b]);
					break;
				case OP_GTE:
					vm->regs[cur.a] = solid_gte(vm->regs[cur.a], vm->regs[cur.b]);
					break;
			}
		}
	} else {
		log_err("Object not a function");
		exit(1);
	}
	return NULL;
}

solid_object *call_method(solid_vm *vm, solid_object *o, solid_object *method)
{
	if (o->type != T_INSTANCE) {
		log_err("Attempt to call method on primitive");
		exit(1);
	} else {
		hash_map *h = (hash_map *) o->data;
		solid_object *m = (solid_object *) get_hash(h, get_str_value(method));
		return solid_eval_bytecode(vm, m);
	}
}
