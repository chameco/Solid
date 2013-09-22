#ifndef SOLID_OBJECT_H
#define SOLID_OBJECT_H

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <cuttle/utils.h>
#include <cuttle/debug.h>

struct solid_object;

typedef enum solid_type {
	T_NULL, //Throw an error when detected.
	T_INSTANCE,
	T_INT,
	T_STR,
	T_BOOL,
	T_LIST,
	T_FUNC,
	T_CFUNC,
	T_NODE,
} solid_type;

typedef struct solid_object {
	solid_type type;
	void *data;
} solid_object;

void set_namespace(solid_object *ns, solid_object *name, solid_object *o);
solid_object *get_namespace(solid_object *ns, solid_object *name);
int namespace_has(solid_object *ns, solid_object *name);
solid_object *make_object();
solid_object *solid_instance();
solid_object *solid_int(int val);
solid_object *solid_str(char *val);
solid_object *solid_bool(int val);
solid_object *solid_list(list_node *l);
solid_object *solid_func();
solid_object *solid_cfunc();
solid_object *solid_node();

void delete_object(solid_object *o);
solid_object *clone_object(solid_object *class);

int get_int_value(solid_object *o);
char *get_str_value(solid_object *o);
int get_bool_value(solid_object *o);

#endif
