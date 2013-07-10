#include <ruminate.h>

#include <glib.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef char *string;

struct __attribute__((packed)) foo {
	short bar;
	string baz;
};

void die_if_error( GError *err ) {
	if( err == NULL ) return;

	fprintf(stderr, "%s\n", err->message);

	exit(EXIT_FAILURE);
}

static void print_basic_type_value( BasicTypeIdentifier id, void *data ) {
	switch( id ) {
		case BASIC_TYPE_INT:
			printf("%d", *((int *) data));
			break;
		case BASIC_TYPE_SHORT:
			printf("%hd", *((short *) data));
			break;
		default:
			printf("\"unknown basic type\"");
	}
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
			case TYPE_CLASS_BUILTIN: {
				BasicType *bt = type_as_basic(member->type, &err);
				die_if_error(err);
				print_basic_type_value(bt->id, &f + member->offset);
				type_unref((Type *) bt);
				break;
			}
			case TYPE_CLASS_TYPEDEF: {
				if( strcmp(member->type->name, "string") == 0 ) {
					printf("\"%s\"", *((char **) (((uint8_t *) &f) + member->offset)));
					break;
				}
				// Fallthrough
			}
			default: {
				printf("\"unknown type '%s' with id %d\"", member->type->name, member->type->id);
			}
		}
		struct_member_unref(member);
		if( i != st->nfields - 1 ) printf(",");
	}
	printf("}\n");

	type_unref((Type *) st);
}
