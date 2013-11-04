#include <stdlib.h>
#include <stdio.h>

#include <glib.h>
#include <ruminate.h>
#include <jansson.h>

#include "json-lib.h"

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

void die_if_error( GError *err ) {
	if( err == NULL ) return;
	fprintf(stderr, "%s\n", err->message);
	exit(EXIT_FAILURE);
}

static json_t *serialize_my_union( JsonState *js, RType *rt, void *value, void *data, GError **error ) {
	(void) js, (void) rt, (void) data, (void) error;
	char v[] = { *((char *) value), '\0' };
	return json_string(v);
}

static JsonSerializer my_union_serializer = {
	.func = serialize_my_union
};

static json_t *serialize_string( JsonState *js, RType *rt, void *value, void *data, GError **error ) {
	(void) js, (void) rt, (void) data, (void) error;
	return json_string(*((char **) value));
}

static JsonSerializer string_serializer = {
	.func = serialize_string
};

int main( int argc, char *argv[] ) {
	GError *err = NULL;
	ruminate_init(&argc, argv, &err);
	die_if_error(err);
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
	RType *typ = ruminate_get_type(foo, &err);
	die_if_error(err);
	JsonState *st = json_state_new();
	json_state_add_serializer(st, g_quark_from_static_string("string"), &string_serializer);
	json_state_add_serializer(st, g_quark_from_static_string("MyUnion"), &my_union_serializer);
	json_t *ser = json_serialize(st, typ, &foo, &err);
	die_if_error(err);
	json_dumpf(ser, stdout, 0);
	printf("\n");
}