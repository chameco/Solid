#include "object.h"

solid_object *SOLID_CLASS_OBJECT;
solid_object *SOLID_CLASS_INTEGER;
solid_object *SOLID_CLASS_STRING;
solid_object *SOLID_CLASS_BOOLEAN;
solid_object *SOLID_CLASS_FUNC;
solid_object *SOLID_CLASS_CLASS;

void set_namespace(solid_object *ns, solid_object *name, solid_object *o)
{
	if (ns->type != T_INSTANCE) {
		log_err("ERROR: Namespace is not an instance");
		exit(1);
	} else {
		hash_map *h = (hash_map *) ns->data;
		set_hash(h, get_str_value(name), (void *) o);
	}
}

solid_object *get_namespace(solid_object *ns, solid_object *name)
{
	if (ns->type != T_INSTANCE) {
		log_err("ERROR: Namespace is not an instance");
		exit(1);
	} else {
		hash_map *h = (hash_map *) ns->data;
		return get_hash(h, get_str_value(name));
	}
}

void initialize_builtin_classes(solid_object *global_ns)
{
	SOLID_CLASS_OBJECT = solid_class(solid_str("Object"), NULL);
	set_namespace(global_ns, solid_str("Object"), SOLID_CLASS_OBJECT);

	SOLID_CLASS_INTEGER = solid_class(solid_str("Integer"), SOLID_CLASS_OBJECT);
	set_namespace(global_ns, solid_str("Integer"), SOLID_CLASS_INTEGER);

	SOLID_CLASS_STRING = solid_class(solid_str("String"), SOLID_CLASS_OBJECT);
	set_namespace(global_ns, solid_str("String"), SOLID_CLASS_STRING);

	SOLID_CLASS_BOOLEAN = solid_class(solid_str("Boolean"), SOLID_CLASS_OBJECT);
	set_namespace(global_ns, solid_str("String"), SOLID_CLASS_BOOLEAN);

	SOLID_CLASS_FUNC = solid_class(solid_str("Function"), SOLID_CLASS_OBJECT);
	set_namespace(global_ns, solid_str("Function"), SOLID_CLASS_FUNC);

	SOLID_CLASS_CLASS = solid_class(solid_str("Class"), SOLID_CLASS_OBJECT);
	set_namespace(global_ns, solid_str("Class"), SOLID_CLASS_CLASS);
}

solid_object *make_object()
{
	solid_object *ret = (solid_object *) malloc(sizeof(solid_object));
	ret->type = T_NULL;
	ret->class = SOLID_CLASS_OBJECT;
	ret->data = NULL;
	return ret;
}

solid_object *solid_instance()
{
	solid_object *ret = make_object();
	ret->type = T_INSTANCE;
	ret->data = (void *) make_hash_map();
	return ret;
}

solid_object *solid_int(int val)
{
	solid_object *ret = make_object();
	ret->type = T_INT;
	ret->class = SOLID_CLASS_INTEGER;
	ret->data = malloc(sizeof(int));
	memcpy(ret->data, &val, sizeof(int));
	return ret;
}

solid_object *solid_str(char *val)
{
	solid_object *ret = make_object();
	ret->type = T_STR;
	ret->class = SOLID_CLASS_STRING;
	ret->data = malloc(strlen(val) + sizeof(char)); //Need the null byte
	strcpy((char *) ret->data, val);
	return ret;
}

solid_object *solid_bool(int val)
{
	solid_object *ret = make_object();
	ret->type = T_BOOL;
	ret->class = SOLID_CLASS_BOOLEAN;
	ret->data = malloc(sizeof(int));
	int temp = (val != 0);
	memcpy(ret->data, &temp, sizeof(int));
	return ret;
}

solid_object *solid_func()
{
	solid_object *ret = make_object();
	ret->type = T_FUNC;
	ret->class = SOLID_CLASS_FUNC;
	ret->data = NULL;
	return ret; //We don't do anything here: all bytecode will be added later
}

solid_object *solid_class(solid_object *name, solid_object *super)
{
	solid_object *ret = make_object();
	ret->type = T_CLASS;
	ret->class = SOLID_CLASS_CLASS;
	ret->data = (void *) solid_instance();
	set_namespace((solid_object *) ret->data, solid_str("__name__"), name);
	if (super != NULL) { //Should only be null for Object
		set_namespace((solid_object *) ret->data, solid_str("__super__"), super);
	}
	return ret;
}

void delete_object(solid_object *o) //More than this is needed to truly delete an object,
{                                   //possibly add a garbage collector in vm?
	if (o->data != NULL) {
		free(o->data);
	}
	free(o);
}

int get_int_value(solid_object *o)
{
	if (o->type == T_INT) {
		return *((int *) o->data);
	}
	log_err("ERROR: Object not of integral type");
	exit(1);
}

char *get_str_value(solid_object *o)
{
	if (o->type == T_STR) {
		return ((char *) o->data);
	}
	log_err("ERROR: Object not of string type");
	exit(1);
}

int get_bool_value(solid_object *o)
{
	if (o->type == T_BOOL) {
		return *((int *) o->data);
	}
	log_err("ERROR: Object not of boolean type");
	exit(1);
}
