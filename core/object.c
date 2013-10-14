#include "object.h"

void solid_set_namespace(solid_object *ns, solid_object *name, solid_object *o)
{
	if (ns->type != T_INSTANCE) {
		debug("ns->type: %d", ns->type);
		log_err("Namespace is not an instance");
		exit(1);
	} else {
		hash_map *h = (hash_map *) ns->data;
		set_hash(h, solid_get_str_value(name), (void *) o);
	}
}

solid_object *solid_get_namespace(solid_object *ns, solid_object *name)
{
	if (ns->type != T_INSTANCE) {
		debug("ns->type: %d", ns->type);
		log_err("Namespace is not an instance");
		exit(1);
	} else {
		hash_map *h = (hash_map *) ns->data;
		solid_object *ret = get_hash(h, solid_get_str_value(name));
		if (ret == NULL) {
			log_err("Variable \"%s\" not in namespace", solid_get_str_value(name));
			exit(1);
		}
		return ret;
	}
}

int solid_namespace_has(solid_object *ns, solid_object *name)
{
	if (ns->type != T_INSTANCE) {
		debug("ns->type: %d", ns->type);
		log_err("Namespace is not an instance");
		exit(1);
	} else {
		hash_map *h = (hash_map *) ns->data;
		solid_object *ret = get_hash(h, solid_get_str_value(name));
		if (ret == NULL) {
			return 0;
		}
		return 1;
	}
}

solid_object *solid_make_object()
{
	solid_object *ret = (solid_object *) malloc(sizeof(solid_object));
	ret->type = T_NULL;
	ret->data_size = 0;
	ret->data = NULL;
	return ret;
}

solid_object *solid_instance()
{
	solid_object *ret = solid_make_object();
	ret->type = T_INSTANCE;
	ret->data_size = sizeof(hash_map);
	ret->data = (void *) make_hash_map();
	return ret;
}

solid_object *solid_int(int val)
{
	solid_object *ret = solid_make_object();
	ret->type = T_INT;
	ret->data_size = sizeof(int);
	ret->data = malloc(sizeof(int));
	memcpy(ret->data, &val, sizeof(int));
	return ret;
}

solid_object *solid_str(char *val)
{
	solid_object *ret = solid_make_object();
	size_t len = strlen(val) + sizeof(char); //Null byte at the end
	ret->type = T_STR;
	ret->data_size = len;
	ret->data = malloc(len);
	strcpy((char *) ret->data, val);
	return ret;
}

solid_object *solid_bool(int val)
{
	solid_object *ret = solid_make_object();
	ret->type = T_BOOL;
	ret->data_size = sizeof(int);
	ret->data = malloc(sizeof(int));
	int temp = (val != 0);
	memcpy(ret->data, &temp, sizeof(int));
	return ret;
}

solid_object *solid_list(list_node *l)
{
	solid_object *ret = solid_make_object();
	ret->type = T_LIST;
	ret->data_size = sizeof(list_node);
	ret->data = l;
	return ret;
}

solid_object *solid_func()
{
	solid_object *ret = solid_make_object();
	ret->type = T_FUNC;
	ret->data_size = 0;
	ret->data = NULL;
	return ret; //We don't do anything here: all bytecode will be added later
}

solid_object *solid_cfunc()
{
	solid_object *ret = solid_make_object();
	ret->type = T_CFUNC;
	ret->data = NULL;
	return ret;
}

solid_object *solid_node()
{
	solid_object *ret = solid_make_object();
	ret->type = T_NODE;
	ret->data = NULL;
	return ret;
}

solid_object *solid_struct(void *val)
{
	solid_object *ret = solid_make_object();
	ret->type = T_STRUCT;
	ret->data = val;
	return ret;
}

void solid_delete_object(solid_object *o) //More than this is needed to truly delete an object,
{                                   //possibly add a garbage collector in vm?
	free(o);
}

solid_object *solid_clone_object(solid_object *class)
{
	if (class->type != T_INSTANCE) {
		log_err("Attempt to clone non-namespace object");
		exit(1);
	} else {
		solid_object *ret = solid_make_object();
		ret->type = T_INSTANCE;
		ret->data = (void *) copy_hash((hash_map *) class->data);
		return ret;
	}
}

int solid_get_int_value(solid_object *o)
{
	if (o->type == T_INT) {
		return *((int *) o->data);
	}
	log_err("Object not of integral type");
	exit(1);
}

char *solid_get_str_value(solid_object *o)
{
	if (o->type == T_STR) {
		return ((char *) o->data);
	}
	log_err("Object not of string type");
	exit(1);
}

int solid_get_bool_value(solid_object *o)
{
	if (o->type == T_BOOL || o->type == T_INT) {
		return *((int *) o->data);
	}
	log_err("Object not of boolean type");
	exit(1);
}

void *solid_get_struct_value(solid_object *o)
{
	if (o->type == T_STRUCT) {
		return o->data;
	}
	log_err("Object not of structure type");
	exit(1);
}
