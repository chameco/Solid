#include <solid/solid.h>
#include <cuttle/debug.h>
#include <cuttle/utils.h>

void solid_map(solid_vm *vm)
{
	solid_object *list = solid_pop_stack(vm);
	if (list->type != T_LIST) {
		log_err("Attempt to map over non-list object");
		exit(1);
	}
	list_node *l = (list_node *) list->data;
	solid_object *f = solid_pop_stack(vm);
	if (f->type != T_FUNC && f->type != T_CFUNC) {
		log_err("Non-function passed to map");
		exit(1);
	}
	list_node *ret = make_list();
	list_node *ins = ret;
	list_node *c;
	for (c = l->next; c->next != NULL; c = c->next) {
		if (c->data != NULL) {
			solid_push_stack(vm, (solid_object *) c->data);
			solid_call_func(vm, f);
			insert_list(ins, vm->regs[255]);
			ins = ins->next;
		}
	}
	vm->regs[255] = solid_list(vm, ret);
}

void solid_reduce(solid_vm *vm)
{
	solid_object *list = solid_pop_stack(vm);
	if (list->type != T_LIST) {
		log_err("Attempt to reduce non-list object");
		exit(1);
	}
	list_node *l = (list_node *) list->data;
	solid_object *acc = solid_pop_stack(vm);
	solid_object *f = solid_pop_stack(vm);
	if (f->type != T_FUNC && f->type != T_CFUNC) {
		log_err("Non-function passed to reduce");
		log_err("Type is %d", f->type);
		exit(1);
	}
	list_node *c;
	for (c = l->next; c->next != NULL; c = c->next) {
		if (c->data != NULL) {
			solid_push_stack(vm, (solid_object *) c->data);
			solid_push_stack(vm, acc);
			solid_call_func(vm, f);
			acc = vm->regs[255];
		}
	}
	vm->regs[255] = acc;
}

void solid_init(solid_vm *vm)
{
	solid_set_namespace(solid_get_current_namespace(vm), solid_str(vm, "map"), solid_define_c_function(vm, solid_map));
	solid_set_namespace(solid_get_current_namespace(vm), solid_str(vm, "reduce"), solid_define_c_function(vm, solid_reduce));
}
