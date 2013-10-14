#include <solid/solid.h>

void solid_plus(solid_vm *vm)
{
	int a = solid_get_int_value(solid_pop_stack(vm));
	int b = solid_get_int_value(solid_pop_stack(vm));
	vm->regs[255] = solid_int(a + b);
}

void solid_init(solid_vm *vm)
{
	solid_set_namespace(solid_get_current_namespace(vm), solid_str("plus"), solid_define_c_function(solid_plus));
}
