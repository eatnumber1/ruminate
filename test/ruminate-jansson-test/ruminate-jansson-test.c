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
	union {
		char b;
		void *v;
	} *u1;
	union MyUnion {
		char b;
		void *v;
	} u2;
	string s;
	enum MyEnum {
		MY_ENUM_VALUE_1,
		MY_ENUM_VALUE_2
	} e;
	int *p;
	char a[3];
};

static void *deserialize_my_union( JsonDeserializerArgs args, void *data, GError **error ) {
	(void) data, (void) error;
	char *ret = r_mem_malloc_fn(args.type, error);
	if( ret == NULL ) return NULL;

	const char *c = json_string_value(args.value);
	*ret = *c;
	return ret;
}

static json_t *serialize_my_union( JsonSerializerArgs args, void *data, GError **error ) {
	(void) data, (void) error;
	char v[] = { *((char *) args.value), '\0' };
	return json_string(v);
}

static JsonHook my_union_hook = {
	.serializer = serialize_my_union,
	.deserializer = deserialize_my_union
};

static void *deserialize_string( JsonDeserializerArgs args, void *data, GError **error ) {
	(void) data, (void) error;
	const char *str = json_string_value(args.value);
	size_t str_len = strlen(str) + 1;

	char **ret = r_mem_malloc_fn(args.type, error);
	if( ret == NULL ) return NULL;
	*ret = r_mem_malloc_sized(char *, str_len, error);
	if( *ret == NULL ) {
		r_mem_unref(ret);
		return NULL;
	}
	memcpy(*ret, str, str_len);

	return ret;
}

static json_t *serialize_string( JsonSerializerArgs args, void *data, GError **error ) {
	(void) data, (void) error;
	return json_string(*((char **) args.value));
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
		.u1 = NULL,
		.u2 = {
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
	json_state_set_flags(st, JSON_FLAG_INVERTABLE);
	json_t *serialized = json_serialize(st, ruminate_get_type(foo, NULL), &foo, NULL);
	json_dumpf(serialized, stdout, 0);
	printf("\n");
	struct MyStruct *_foo = json_deserialize(st, serialized, NULL);
	printf("struct MyStruct {");
	printf(" .i = %d,", _foo->i);
	printf(" .u1 = %p,", _foo->u1);
	printf(" .u2 = { .b = '%c' },", _foo->u2.b);
	printf(" .s = \"%s\",", _foo->s);
	printf(" .e = %d,", _foo->e);
	printf(" .p = %p (%d),", _foo->p, *_foo->p);
	printf(" .a = [%d, %d, %d]", _foo->a[0], _foo->a[1], _foo->a[2]);
	printf(" };\n");
	r_mem_unref(_foo->s);
	r_mem_unref(_foo->p);
	r_mem_unref(_foo);
}
