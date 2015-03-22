#ifndef SOLID_OBJECT_H
#define SOLID_OBJECT_H

#include <stdlib.h>
#include <stdbool.h>
#include <cuttle/utils.h>

typedef struct solid_object solid_object;

#include "vm.h"

typedef enum solid_type {
	T_NULL, //Throw an error when detected.
	T_INSTANCE,
	T_INT,
	T_DOUBLE,
	T_STR,
	T_BOOL,
	T_LIST,
	T_FUNC,
	T_CFUNC,
	T_STRUCT, //Arbitrary C struct.
} solid_type;

typedef union solid_object_data {
	hash_map *instance;
	int i;
	double d;
	char *str;
	bool b;
	list_node *list;
	void *func;
	void (*cfunc)(solid_vm *);
	void *cstruct;
} solid_object_data;

struct solid_object {
	solid_type type;
	unsigned char marked;
	size_t data_size;
	solid_object_data data;
};

void solid_set_namespace(solid_object *ns, solid_object *name, solid_object *o);
solid_object *solid_get_namespace(solid_object *ns, solid_object *name);
int solid_namespace_has(solid_object *ns, solid_object *name);
solid_object *solid_make_object(solid_vm *vm);
solid_object *solid_instance(solid_vm *vm);
solid_object *solid_int(solid_vm *vm, int val);
solid_object *solid_double(solid_vm *vm, double val);
solid_object *solid_str(solid_vm *vm, char *val);
solid_object *solid_bool(solid_vm *vm, bool val);
solid_object *solid_list(solid_vm *vm, list_node *l);
solid_object *solid_func(solid_vm *vm);
solid_object *solid_cfunc(solid_vm *vm);
solid_object *solid_struct(solid_vm *vm, void *val);

void solid_mark_object(solid_object *o, unsigned char m);
void solid_mark_list(list_node *l, unsigned char m);
void solid_mark_hash(hash_map *l, unsigned char m);

void solid_delete_object(solid_vm *vm, solid_object *o);
void solid_delete_list(solid_vm *vm, list_node *l);
void solid_delete_hash(solid_vm *vm, hash_map *l);
solid_object *solid_clone_object(solid_vm *vm, solid_object *class);

int solid_get_int_value(solid_object *o);
double solid_get_double_value(solid_object *o);
char *solid_get_str_value(solid_object *o);
bool solid_get_bool_value(solid_object *o);
list_node *solid_get_list_value(solid_object *o);
void *solid_get_struct_value(solid_object *o);

#endif
