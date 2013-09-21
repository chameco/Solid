#include <stdlib.h>
#include <stddef.h>
#include <solid/solid.h>

void solid_plus(solid_vm *vm)
{
	int a = get_int_value(pop_stack(vm));
	int b = get_int_value(pop_stack(vm));
	vm->regs[255] = solid_int(a + b);
}
void solid_init(solid_vm *vm)
{
	set_namespace(get_current_namespace(vm), solid_str("plus"), define_c_function(solid_plus));
}
