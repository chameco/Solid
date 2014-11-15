#include "object.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <cuttle/debug.h>

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

solid_object *solid_make_object(solid_vm *vm)
{
	solid_object *ret = (solid_object *) malloc(sizeof(solid_object));
	ret->type = T_NULL;
	ret->marked = 0;
	ret->data_size = 0;
	ret->data = NULL;
	solid_gc_add_object(vm, ret);
	return ret;
}

solid_object *solid_instance(solid_vm *vm)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_INSTANCE;
	ret->data_size = sizeof(hash_map);
	ret->data = (void *) make_hash_map();
	return ret;
}

solid_object *solid_int(solid_vm *vm, int val)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_INT;
	ret->data_size = sizeof(int);
	ret->data = malloc(sizeof(int));
	memcpy(ret->data, &val, sizeof(int));
	return ret;
}

solid_object *solid_double(solid_vm *vm, double val)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_DOUBLE;
	ret->data_size = sizeof(double);
	ret->data = malloc(sizeof(double));
	memcpy(ret->data, &val, sizeof(double));
	return ret;
}

solid_object *solid_str(solid_vm *vm, char *val)
{
	solid_object *ret = solid_make_object(vm);
	size_t len = strlen(val) + sizeof(char); //Null byte at the end
	ret->type = T_STR;
	ret->data_size = len;
	ret->data = malloc(len);
	strcpy((char *) ret->data, val);
	return ret;
}

solid_object *solid_bool(solid_vm *vm, int val)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_BOOL;
	ret->data_size = sizeof(int);
	ret->data = malloc(sizeof(int));
	int temp = (val != 0);
	memcpy(ret->data, &temp, sizeof(int));
	return ret;
}

solid_object *solid_list(solid_vm *vm, list_node *l)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_LIST;
	ret->data_size = sizeof(list_node);
	ret->data = l;
	return ret;
}

solid_object *solid_func(solid_vm *vm)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_FUNC;
	ret->data_size = 0;
	ret->data = NULL;
	return ret; //We don't do anything here: all bytecode will be added later
}

solid_object *solid_cfunc(solid_vm *vm)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_CFUNC;
	ret->data = NULL;
	return ret;
}

solid_object *solid_node(solid_vm *vm)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_NODE;
	ret->data = NULL;
	return ret;
}

solid_object *solid_struct(solid_vm *vm, void *val)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_STRUCT;
	ret->data = val;
	return ret;
}

void solid_mark_object(solid_object *o, unsigned char m)
{
	if (o != NULL) {
		if (o->marked != 0) {
			return;
		}

		o->marked = m;

		switch (o->type) {
			case T_INSTANCE:
				solid_mark_hash((hash_map *) o->data, m);
				break;
			case T_LIST:
				solid_mark_list((list_node *) o->data, m);
				break;
			case T_FUNC:
				solid_mark_object(((solid_function *) o->data)->closure, m);
				break;
			default:
				break;
		}
	}
}

void solid_mark_list(list_node *l, unsigned char m)
{
	if (l != NULL) {
		list_node *c;
		for (c = l->next; c != NULL; c = c->next) {
			if (c->data != NULL) {
				solid_mark_object((solid_object *) c->data, m);
			}
		}
	}
}

void solid_mark_hash(hash_map *h, unsigned char m)
{
	int c = 0;
	list_node *l;
	list_node *cur;
	hash_val *hv;
	for (c = 0; c < 256; ++c) {
		l = h->buckets[c];
		if (l != NULL) {
			for (cur = l->next; cur != NULL; cur = cur->next) {
				if (cur->data != NULL) {
					hv = (hash_val *) cur->data;
					solid_mark_object((solid_object *) hv->val, m);
				}
			}
		}
	}
}

void solid_delete_object(solid_vm *vm, solid_object *o)
{
	switch (o->type) {
		case T_NULL:
			break;
		case T_CFUNC:
			return;
		case T_NODE:
			break;
		case T_STRUCT:
			break;
		case T_INSTANCE:
			//solid_delete_hash(vm, (hash_map *) o->data);
			break;
		case T_LIST:
			solid_delete_list(vm, (list_node *) o->data);
			break;
		case T_INT:
		case T_DOUBLE:
		case T_STR:
		case T_BOOL:
			free(o->data);
			break;
		case T_FUNC:
			return;
	}
	//free(o);
}

void solid_delete_list(solid_vm *vm, list_node *l)
{
	if (l != NULL) {
		list_node *c;
		for (c = l->next; c != NULL; c = c->next) {
			if (c->data != NULL) {
				solid_delete_object(vm, (solid_object *) c->data);
			}
			free(c);
		}
	}
	free(l);
}

void solid_delete_hash(solid_vm *vm, hash_map *h)
{
	int c = 0;
	list_node *l;
	list_node *cur;
	hash_val *hv;
	for (c = 0; c < 256; ++c) {
		l = h->buckets[c];
		if (l != NULL) {
			for (cur = l->next; cur != NULL; cur = cur->next) {
				if (cur->data != NULL) {
					hv = (hash_val *) cur->data;
					solid_delete_object(vm, (solid_object *) hv->val);
					free(hv);
				}
				free(cur);
			}
		}
		free(l);
	}
}

solid_object *solid_clone_object(solid_vm *vm, solid_object *class)
{
	if (class->type != T_INSTANCE) {
		log_err("Attempt to clone non-namespace object");
		exit(1);
	} else {
		solid_object *ret = solid_make_object(vm);
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

double solid_get_double_value(solid_object *o)
{
	if (o->type == T_DOUBLE) {
		return *((double *) o->data);
	} else if (o->type == T_INT) {
		return (double) *((int *) o->data);
	}
	log_err("Object not of numeric type");
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

list_node *solid_get_list_value(solid_object *o)
{
	if (o->type == T_LIST) {
		return (list_node *) o->data;
	}
	log_err("Object not of list type");
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
