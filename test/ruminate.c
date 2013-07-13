#include <ruminate.h>

#include <glib.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define print_json_for_type(expr, err) ({ \
	GError **_err = (err); \
	typeof(expr) __expr = (expr); \
	Type *_type = rumination_get_type(__expr, _err); \
	bool ret = _type == NULL ? false : _print_json_for_type(_type, &__expr, _err); \
	type_unref(_type); \
	ret; \
})

typedef char *string;

typedef struct __attribute__((packed)) MyStruct {
	short a_short;
	string a_string;
	struct {
		int an_int;
	} a_struct;
} MyTypedef;

void die_if_error( GError *err ) {
	if( err == NULL ) return;

	fprintf(stderr, "%s\n", err->message);

	exit(EXIT_FAILURE);
}

static bool _print_json_for_primitive( Type *type, void *data, GError **err ) {
	PrimitiveType *pt = type_as_primitive(type, err);
	if( pt == NULL ) return false;

	switch( pt->id ) {
		case PRIMITIVE_TYPE_INT:
			printf("%d", *((int *) data));
			break;
		case PRIMITIVE_TYPE_SHORT:
			printf("%hd", *((short *) data));
			break;
		case PRIMITIVE_TYPE_CHAR:
			printf("%c", *((char *) data));
			break;
		case PRIMITIVE_TYPE_SIGNEDCHAR:
			printf("%hhd", *((signed char *) data));
			break;
		case PRIMITIVE_TYPE_UNSIGNEDCHAR:
			printf("%hhu", *((unsigned char *) data));
			break;
		default:
			printf("\"unknown primitive type\"");
	}

	type_unref((Type *) pt);
	return true;
}

static bool _print_json_for_type( Type *type, void *data, GError **err );

static bool _print_json_for_struct( Type *type, void *data, GError **err ) {
	StructType *st = type_as_struct(type, err);
	if( st == NULL ) goto out_type_as_struct;


	printf("{");
	for( size_t i = 0; i < st->nfields; i++ ) {
		StructMember *member = struct_type_field_at_index(st, i, err);
		if( member == NULL ) goto out_struct_type_field_at_index;

		printf("\"%s\":", member->name);

		if( !
			_print_json_for_type(
				member->type,
				((uint8_t *) data) + member->offset,
				err
			)
		) goto out__print_json_for_type;

		struct_member_unref(member);
		if( i != st->nfields - 1 ) printf(",");
		continue;

out__print_json_for_type:
		struct_member_unref(member);
out_struct_type_field_at_index:
		goto out_loop_exit;
	}
	printf("}");

	type_unref((Type *) st);
	return true;

out_loop_exit:
	type_unref((Type *) st);
out_type_as_struct:
	return false;
}

static bool _print_json_for_string( Type *type, void *data, GError **err ) {
	printf("\"%s\"", *((char **) data));
	return true;
}

static bool print_function_type( Type *type, GError **err ) {
	FunctionType *ft = type_as_function(type, err);
	if( ft == NULL ) return false;

	Type *rtype = function_return_type(ft, err);
	if( rtype == NULL ) return false; // TODO: Clean up resources

	printf("(%s (", rtype->name);

	type_unref(rtype);

	for( size_t i = 0; i < ft->narguments; i++ ) {
		Type *arg = function_argument_type_at_index(ft, i, err);
		if( arg == NULL ) return false; // TODO: Clean up resources

		printf(" %s", arg->name);
		if( i != ft->narguments - 1 ) printf(",");

		type_unref(arg);
	}

	printf(" ))");

	type_unref((Type *) ft);

	return true;
}

static bool _print_json_for_type( Type *type, void *data, GError **err ) {
	// Strings are special cased.
	if( type->id == TYPE_CLASS_TYPEDEF && strcmp(type->name, "string") == 0 )
		return _print_json_for_string(type, data, err);

	type = type_as_canonical(type, err);
	if( type == NULL ) return false;

	bool ret;
	switch( type->id ) {
		case TYPE_CLASS_PRIMITIVE:
			ret = _print_json_for_primitive(type, data, err);
			break;
		case TYPE_CLASS_STRUCT:
			ret = _print_json_for_struct(type, data, err);
			break;
		case TYPE_CLASS_FUNCTION:
			g_assert(false);
		case TYPE_CLASS_ARRAY:
		case TYPE_CLASS_ENUMERATION:
		case TYPE_CLASS_POINTER:
		case TYPE_CLASS_TYPEDEF:
		case TYPE_CLASS_UNION:
		default:
			fprintf(stderr, "Unknown type with id %d\n", type->id);
			abort();
	}

	type_unref(type);
	return ret;
}

int main( int argc, char *argv[] ) {
	GError *err = NULL;
	rumination_init(&argc, argv, &err);
	die_if_error(err);

	MyTypedef f = {
		.a_short = 1,
		.a_string = "hello",
		.a_struct = {
			.an_int = 100
		}
	};

	print_json_for_type(f, &err);
	die_if_error(err);

	printf("\n");

	Type *type = rumination_get_type(&print_function_type, &err);
	die_if_error(err);

	Type *pointee = type_pointee(type, &err);
	die_if_error(err);
	type_unref(type);

	print_function_type(pointee, &err);
	die_if_error(err);

	type_unref(type);

	printf("\n");
}
