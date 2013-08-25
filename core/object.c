#include "object.h"

solid_object *SOLID_CLASS_OBJECT;
solid_object *SOLID_CLASS_INTEGER;
solid_object *SOLID_CLASS_STRING;
solid_object *SOLID_CLASS_BOOLEAN;
solid_object *SOLID_CLASS_FUNC;
solid_object *SOLID_CLASS_NODE;

void set_namespace(solid_object *ns, solid_object *name, solid_object *o)
{
	if (ns->type != T_INSTANCE) {
		log_err("Namespace is not an instance");
		exit(1);
	} else {
		hash_map *h = (hash_map *) ns->data;
		if (get_hash(h, get_str_value(name)) != NULL) {
			log_err("Attempt to redefine variable \"%s\"", get_str_value(name));
			exit(1);
		}
		set_hash(h, get_str_value(name), (void *) o);
	}
}

solid_object *get_namespace(solid_object *ns, solid_object *name)
{
	if (ns->type != T_INSTANCE) {
		log_err("Namespace is not an instance");
		exit(1);
	} else {
		hash_map *h = (hash_map *) ns->data;
		solid_object *ret = get_hash(h, get_str_value(name));
		if (ret == NULL) {
			log_err("Variable \"%s\" not in namespace", get_str_value(name));
			exit(1);
		}
		return ret;
	}
}

void initialize_builtin_classes(solid_object *global_ns) //Checks allow us to initialize in any namespace while retaining references
{
	SOLID_CLASS_OBJECT = SOLID_CLASS_OBJECT == NULL ? solid_class(NULL)
		: SOLID_CLASS_OBJECT;
	set_namespace(global_ns, solid_str("Object"), SOLID_CLASS_OBJECT);

	SOLID_CLASS_INTEGER = SOLID_CLASS_INTEGER == NULL ? solid_class(SOLID_CLASS_OBJECT)
		: SOLID_CLASS_INTEGER;
	set_namespace(global_ns, solid_str("Integer"), SOLID_CLASS_INTEGER);

	SOLID_CLASS_STRING = SOLID_CLASS_STRING == NULL ? solid_class(SOLID_CLASS_OBJECT)
		: SOLID_CLASS_STRING;
	set_namespace(global_ns, solid_str("String"), SOLID_CLASS_STRING);

	SOLID_CLASS_BOOLEAN = SOLID_CLASS_BOOLEAN == NULL ? solid_class(SOLID_CLASS_OBJECT)
		: SOLID_CLASS_BOOLEAN;
	set_namespace(global_ns, solid_str("Boolean"), SOLID_CLASS_BOOLEAN);

	SOLID_CLASS_FUNC = SOLID_CLASS_FUNC == NULL ? solid_class(SOLID_CLASS_OBJECT)
		: SOLID_CLASS_FUNC;
	set_namespace(global_ns, solid_str("Function"), SOLID_CLASS_FUNC);
}

solid_object *get_builtin_class_object()
{
	return SOLID_CLASS_OBJECT;
}

solid_object *get_builtin_class_integer()
{
	return SOLID_CLASS_INTEGER;
}

solid_object *get_builtin_class_string()
{
	return SOLID_CLASS_STRING;
}

solid_object *get_builtin_class_boolean()
{
	return SOLID_CLASS_BOOLEAN;
}

solid_object *get_builtin_class_func()
{
	return SOLID_CLASS_FUNC;
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

solid_object *solid_class(solid_object *super)
{
	solid_object *ret;
	if (super != NULL) { //Should only be null for Object
		ret = (void *) instance_from_class(super);
	} else {
		ret = (void *) solid_instance();
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

solid_object *instance_from_class(solid_object *class)
{
	if (class->type != T_INSTANCE) {
		log_err("Attempt to instantiate primitive");
		exit(1);
	} else {
		solid_object *ret = make_object();
		ret->type = T_INSTANCE;
		ret->class = class;
		ret->data = (void *) copy_hash((hash_map *) class->data);
		return ret;
	}
}

int get_int_value(solid_object *o)
{
	if (o->type == T_INT) {
		return *((int *) o->data);
	}
	log_err("Object not of integral type");
	exit(1);
}

char *get_str_value(solid_object *o)
{
	if (o->type == T_STR) {
		return ((char *) o->data);
	}
	log_err("Object not of string type");
	exit(1);
}

int get_bool_value(solid_object *o)
{
	if (o->type == T_BOOL || o->type == T_INT) {
		return *((int *) o->data);
	}
	log_err("Object not of boolean type");
	exit(1);
}
