#include <solid/solid.h>
#include <math.h>

void solid_sin(solid_vm *vm)
{
	double x = solid_get_double_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, sin(x));
}

void solid_cos(solid_vm *vm)
{
	double x = solid_get_double_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, cos(x));
}

void solid_tan(solid_vm *vm)
{
	double x = solid_get_double_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, tan(x));
}

void solid_sqrt(solid_vm *vm)
{
	double x = solid_get_double_value(solid_pop_stack(vm));
	vm->regs[255] = solid_double(vm, sqrt(x));
}

void solid_init(solid_vm *vm)
{
	solid_set_namespace(solid_get_current_namespace(vm), solid_str(vm, "sin"), solid_define_c_function(vm, solid_sin));
	solid_set_namespace(solid_get_current_namespace(vm), solid_str(vm, "cos"), solid_define_c_function(vm, solid_cos));
	solid_set_namespace(solid_get_current_namespace(vm), solid_str(vm, "tan"), solid_define_c_function(vm, solid_tan));
	solid_set_namespace(solid_get_current_namespace(vm), solid_str(vm, "sqrt"), solid_define_c_function(vm, solid_sqrt));
}
