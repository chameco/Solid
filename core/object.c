#include "object.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "utils.h"

void solid_set_namespace(solid_object *ns, solid_object *name, solid_object *o)
{
	if (ns->type != T_INSTANCE) {
		debug("ns->type: %d", ns->type);
		log_err("Namespace is not an instance");
		exit(1);
	} else {
		hash_map *h = ns->data.instance;
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
		hash_map *h = ns->data.instance;
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
		hash_map *h = ns->data.instance;
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
	solid_gc_add_object(vm, ret);
	return ret;
}

solid_object *solid_instance(solid_vm *vm)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_INSTANCE;
	ret->data_size = sizeof(hash_map *);
	ret->data.instance = make_hash_map();
	return ret;
}

solid_object *solid_int(solid_vm *vm, int val)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_INT;
	ret->data_size = sizeof(int);
	ret->data.i = val;
	return ret;
}

solid_object *solid_double(solid_vm *vm, double val)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_DOUBLE;
	ret->data_size = sizeof(double);
	ret->data.d = val;
	return ret;
}

solid_object *solid_str(solid_vm *vm, char *val)
{
	solid_object *ret = solid_make_object(vm);
	size_t len = strlen(val) + sizeof(char); //Null byte at the end
	ret->type = T_STR;
	ret->data_size = len;
	ret->data.str = malloc(len);
	strcpy(ret->data.str, val);
	return ret;
}

solid_object *solid_bool(solid_vm *vm, bool val)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_BOOL;
	ret->data_size = sizeof(int);
	ret->data.b = val;
	return ret;
}

solid_object *solid_list(solid_vm *vm, list_node *l)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_LIST;
	ret->data_size = sizeof(list_node *);
	ret->data.list = l;
	return ret;
}

solid_object *solid_func(solid_vm *vm)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_FUNC;
	ret->data_size = 0;
	return ret; //We don't do anything here: all bytecode will be added later
}

solid_object *solid_cfunc(solid_vm *vm)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_CFUNC;
	return ret;
}

solid_object *solid_struct(solid_vm *vm, void *val)
{
	solid_object *ret = solid_make_object(vm);
	ret->type = T_STRUCT;
	ret->data.cstruct = val;
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
				solid_mark_hash(o->data.instance, m);
				break;
			case T_LIST:
				solid_mark_list(o->data.list, m);
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
		case T_INSTANCE:
			//solid_delete_hash(vm, o->data.instance);
			break;
		case T_LIST:
			solid_delete_list(vm, o->data.list);
			break;
		case T_INT:
		case T_DOUBLE:
		case T_STR:
		case T_BOOL:
			break;
		case T_FUNC:
		case T_CFUNC:
			return;
		case T_STRUCT:
			break;
	}
	free(o);
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
	solid_object *ret;
	switch (class->type) {
		case T_INSTANCE:
			ret = solid_make_object(vm);
			ret->type = T_INSTANCE;
			ret->data_size = sizeof(hash_map *);
			ret->data.instance = solid_clone_hash(vm, class->data.instance);
			break;
		case T_LIST:
			ret = solid_list(vm, solid_clone_list(vm, class->data.list));
			break;
		case T_FUNC:
		case T_CFUNC:
			ret = class;
			break;
		default:
			ret = solid_make_object(vm);
			ret->type = class->type;
			ret->marked = class->marked;
			ret->data_size = class->data_size;
			ret->data = class->data;
			break;
	}
	return ret;
}

list_node *solid_clone_list(solid_vm *vm, list_node *l)
{
	list_node *ret = make_list();
	if (l != NULL) {
		list_node *c;
		for (c = l->next; c != NULL; c = c->next) {
			if (c->data != NULL) {
				insert_list(ret, solid_clone_object(vm, (solid_object *) c->data));
			}
		}
	}
	return ret;
}

hash_map *solid_clone_hash(solid_vm *vm, hash_map *h)
{
	int c = 0;
	hash_map *ret = make_hash_map();
	list_node *l;
	for (c = 0; c < 256; ++c) {
		l = h->buckets[c];
		if (l != NULL) {
			ret->buckets[c] = make_list();
			if (l != NULL) {
				list_node *cur;
				for (cur = l->next; cur != NULL; cur = cur->next) {
					if (cur->data != NULL) {
						hash_val *hv = (hash_val *) malloc(sizeof(hash_val));
						strncpy(hv->key, ((hash_val *) cur->data)->key, 256);
						hv->val = solid_clone_object(vm, (solid_object *) ((hash_val *) cur->data)->val);
						insert_list(ret->buckets[c], hv);
					}
				}
			}
		}
	}
	return ret;
}


int solid_get_int_value(solid_object *o)
{
	if (o->type == T_INT) {
		return o->data.i;
	}
	log_err("Object not of integral type");
	exit(1);
}

double solid_get_double_value(solid_object *o)
{
	if (o->type == T_DOUBLE) {
		return o->data.d;
	} else if (o->type == T_INT) {
		return (double) o->data.i;
	}
	log_err("Object not of numeric type");
	exit(1);
}

char *solid_get_str_value(solid_object *o)
{
	if (o->type == T_STR) {
		return o->data.str;
	}
	log_err("Object not of string type");
	exit(1);
}

bool solid_get_bool_value(solid_object *o)
{
	if (o->type == T_BOOL) {
		return o->data.b;
	} else if (o->type == T_INT) {
		return (bool) o->data.i;
	}
	log_err("Object not of boolean type");
	exit(1);
}

list_node *solid_get_list_value(solid_object *o)
{
	if (o->type == T_LIST) {
		return o->data.list;
	}
	log_err("Object not of list type");
	exit(1);
}

void *solid_get_struct_value(solid_object *o)
{
	if (o->type == T_STRUCT) {
		return o->data.list;
	}
	log_err("Object not of structure type");
	exit(1);
}
