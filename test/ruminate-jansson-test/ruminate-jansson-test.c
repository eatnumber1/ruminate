#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>
#include <ruminate.h>
#include <jansson.h>
#include <ruminate-jansson.h>

typedef char *string;

struct MyStruct {
	int i;
	union MyUnion {
		char b;
		void *v;
	} u;
	string s;
	enum MyEnum {
		MY_ENUM_VALUE_1,
		MY_ENUM_VALUE_2
	} e;
	int *p;
	char a[3];
};

static void *deserialize_my_union( JsonState *js, RType *rt, json_t *value, void *data, GError **error ) {
	(void) js, (void) data, (void) error;
	char *ret = r_mem_malloc_fn(rt, error);
	if( ret == NULL ) return NULL;

	const char *c = json_string_value(value);
	*ret = *c;
	return ret;
}

static json_t *serialize_my_union( JsonState *js, RType *rt, void *value, void *data, GError **error ) {
	(void) js, (void) rt, (void) data, (void) error;
	char v[] = { *((char *) value), '\0' };
	return json_string(v);
}

static JsonHook my_union_hook = {
	.serializer = serialize_my_union,
	.deserializer = deserialize_my_union
};

static void *deserialize_string( JsonState *js, RType *rt, json_t *value, void *data, GError **error ) {
	(void) js, (void) data, (void) error;
	const char *str = json_string_value(value);
	size_t str_len = strlen(str) + 1;

	char **ret = r_mem_malloc_fn(rt, error);
	if( ret == NULL ) return NULL;
	*ret = r_mem_malloc_sized(char *, str_len, error);
	if( *ret == NULL ) {
		r_mem_unref(ret);
		return NULL;
	}
	memcpy(*ret, str, str_len);

	return ret;
}

static json_t *serialize_string( JsonState *js, RType *rt, void *value, void *data, GError **error ) {
	(void) js, (void) rt, (void) data, (void) error;
	return json_string(*((char **) value));
}

static JsonHook string_hook = {
	.serializer = serialize_string,
	.deserializer = deserialize_string
};

int main( int argc, char *argv[] ) {
	(void) argc;
	ruminate_init(argv[0], NULL);
	int ipt = 2;
	struct MyStruct foo = {
		.i = 1,
		.u = {
			.b = 'c'
		},
		.s = "hello world!",
		.e = MY_ENUM_VALUE_2,
		.p = &ipt,
		.a = { 1, 2, 3 }
	};
	JsonState *st = json_state_new();
	json_state_add_hook(st, g_quark_from_static_string("string"), &string_hook);
	json_state_add_hook(st, g_quark_from_static_string("MyUnion"), &my_union_hook);
	json_state_set_flags(st, JSON_FLAG_BIJECTIVE);
	//json_dumpf(json_serialize(st, ruminate_get_type(foo, NULL), &foo, NULL), stdout, 0);
	json_t *serialized = json_serialize(st, ruminate_get_type(foo, NULL), &foo, NULL);
	json_dumpf(serialized, stdout, 0);
	printf("\n");
	GError *err = NULL;
	struct MyStruct *_foo = json_deserialize(st, serialized, &err);
	if( _foo == NULL ) {
		fprintf(stderr, "%s\n", err->message);
		exit(EXIT_FAILURE);
	}
	printf("struct MyStruct {\n");
	printf("\t.i = %d,\n", _foo->i);
	printf("\t.u = { .b = '%c' },\n", _foo->u.b);
	printf("\t.s = \"%s\",\n", _foo->s);
	printf("\t.e = %d,\n", _foo->e);
	printf("\t.p = %p (%d),\n", _foo->p, *_foo->p);
	printf("\t.a = [%d, %d, %d]\n", _foo->a[0], _foo->a[1], _foo->a[2]);
	printf("};\n");
	r_mem_unref(_foo->s);
	r_mem_unref(_foo->p);
	r_mem_unref(_foo);
}
