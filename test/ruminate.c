#include <ruminate.h>

#include <glib.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct foo {
	int bar;
	char *baz;
} foo;

void die_if_error( GError *err ) {
	if( err == NULL ) return;

	fprintf(stderr, "%s\n", err->message);

	exit(EXIT_FAILURE);
}

int main( int argc, char *argv[] ) {
	GError *err = NULL;
	rumination_init(&argc, argv, &err);
	die_if_error(err);

	struct foo f = {
		.bar = 1,
		.baz = "hello"
	};

	Type *type = rumination_get_type(f, &err);
	die_if_error(err);

	StructType *st = type_as_struct(type, &err);
	die_if_error(err);
	type_unref(type);

	printf("%s\n", st->type.name);

	printf("{");
	for( size_t i = 0; i < st->nfields; i++ ) {
		StructMember *member = struct_type_field_at_index(st, i, &err);
		die_if_error(err);
		printf("\"%s\":", member->name);
		switch( member->type->id ) {
			case TYPE_CLASS_BUILTIN:
				printf("\"builtin\"");
				break;
			default:
				printf("\"unknown\"");
		}
		struct_member_unref(member);
	}
	printf("}\n");

	type_unref((Type *) st);
}
