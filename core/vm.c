#include "vm.h"

solid_vm *make_solid_vm()
{
	solid_vm *ret = (solid_vm *) malloc(sizeof(solid_vm));
	ret->stack = make_list();
	ret->global_ns = solid_instance(); 
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
		log_err("ERROR: Stack is empty");
		exit(1);
	} else {
		return (solid_object *) n->data;
	}
}

solid_object *define_function(solid_bytecode *inslist)
{
	solid_object *ret = solid_func();
	ret->data = (void *) inslist;
	return ret;
}

solid_object *solid_add(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("ERROR: Attempt to do apply operator on invalid types");
	}
	return solid_int(get_int_value(a) + get_int_value(b));
}

solid_object *solid_sub(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("ERROR: Attempt to do apply operator on invalid types");
	}
	return solid_int(get_int_value(a) - get_int_value(b));
}

solid_object *solid_mul(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("ERROR: Attempt to do apply operator on invalid types");
	}
	return solid_int(get_int_value(a) * get_int_value(b));
}

solid_object *solid_div(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("ERROR: Attempt to do apply operator on invalid types");
	}
	return solid_int(get_int_value(a) / get_int_value(b));
}

solid_object *solid_eq(solid_object *a,	solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("ERROR: Attempt to do apply operator on invalid types");
	}
	return solid_bool(get_int_value(a) == get_int_value(b));
}

solid_object *solid_not(solid_object *o)
{
	if (o->type != T_BOOL) {
		log_err("ERROR: Attempt to do apply operator on invalid types");
	}
	return solid_bool(!get_bool_value(o));
}

solid_object *solid_lt(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("ERROR: Attempt to do apply operator on invalid types");
	}
	return solid_bool(get_int_value(a) < get_int_value(b));
}

solid_object *solid_lte(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("ERROR: Attempt to do apply operator on invalid types");
	}
	return solid_bool(get_int_value(a) <= get_int_value(b));
}

solid_object *solid_gt(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("ERROR: Attempt to do apply operator on invalid types");
	}
	return solid_bool(get_int_value(a) > get_int_value(b));
}

solid_object *solid_gte(solid_object *a, solid_object *b)
{
	if (a->type != T_INT || b->type != T_INT) {
		log_err("ERROR: Attempt to do apply operator on invalid types");
	}
	return solid_bool(get_int_value(a) >= get_int_value(b));
}

solid_object *solid_eval_bytecode(solid_vm *vm, solid_object *func)
{
	if (func->type == T_FUNC) {
		solid_bytecode *inslist = (solid_bytecode *) func->data;
		solid_bytecode cur;
		int pos;
		for (pos = 0, cur = inslist[pos]; cur.ins != OP_END; cur = inslist[++pos]) {
			switch(cur.ins) {
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
					vm->regs[cur.a] = get_namespace(vm->regs[cur.a], vm->regs[cur.b]);
					break;
				case OP_SET:
					set_namespace(vm->regs[cur.a], solid_str(cur.meta), vm->regs[cur.b]);
					break;
				case OP_PUSHINT:
					push_stack(vm, solid_int(cur.a));
					break;
				case OP_PUSHSTR:
					push_stack(vm, solid_str(cur.meta));
					break;
				case OP_GLOBALNS:
					vm->regs[cur.a] = vm->global_ns;
					break;
				case OP_DEF:
					break;
				case OP_CLASS:
					break;
				case OP_JMP:
					pos += cur.a;
					break;
				case OP_JMPIF:
					if (get_bool_value(vm->regs[cur.b])) {
						pos += cur.a;
					}
					break;
				case OP_CALL:
					solid_eval_bytecode(vm, vm->regs[cur.a]);
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
		log_err("ERROR: Object not a function");
		exit(1);
	}
}

solid_object *call_method(solid_vm *vm, solid_object *o, solid_object *method)
{
	if (o->type != T_CLASS) {
		return call_method(vm, o->class, method);
	} else {
		char *mname = get_str_value(method);
		hash_map *h = (hash_map *) o->data;
		solid_object *m = (solid_object *) get_hash(h, mname);
		if (m == NULL) {
			solid_object *super = (solid_object *) get_hash(h, "__super__");
			if (super == NULL) {
				log_err("ERROR: Method does not exist");
				exit(1);
			} else {
				return call_method(vm, super, method);
			}
		} else {
			return solid_eval_bytecode(vm, m);
		}
	}
}

