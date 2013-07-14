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
	RType *_type = rumination_get_type(__expr, _err); \
	bool ret = _type == NULL ? false : _print_json_for_type(_type, &__expr, _err); \
	r_type_unref(_type); \
	ret; \
})

typedef char *string;

typedef struct __attribute__((packed)) MyStruct {
	short a_short;
	string a_string;
	struct {
		int an_int;
	} a_struct;
	int *an_int_ptr;
} MyTypedef;

void die_if_error( GError *err ) {
	if( err == NULL ) return;

	fprintf(stderr, "%s\n", err->message);

	exit(EXIT_FAILURE);
}

static bool _print_json_for_primitive( RType *type, void *data, GError **err ) {
	RPrimitiveType *pt = r_type_as_primitive(type, err);
	if( pt == NULL ) return false;

	switch( pt->id ) {
		case R_PRIMITIVE_TYPE_INT:
			printf("%d", *((int *) data));
			break;
		case R_PRIMITIVE_TYPE_SHORT:
			printf("%hd", *((short *) data));
			break;
		case R_PRIMITIVE_TYPE_CHAR:
			printf("%c", *((char *) data));
			break;
		case R_PRIMITIVE_TYPE_SIGNEDCHAR:
			printf("%hhd", *((signed char *) data));
			break;
		case R_PRIMITIVE_TYPE_UNSIGNEDCHAR:
			printf("%hhu", *((unsigned char *) data));
			break;
		default:
			printf("\"unknown primitive type\"");
	}

	r_type_unref((RType *) pt);
	return true;
}

static bool _print_json_for_type( RType *type, void *data, GError **err );

static bool _print_json_for_struct( RType *type, void *data, GError **err ) {
	RStructType *st = r_type_as_struct(type, err);
	if( st == NULL ) goto out_type_as_struct;


	printf("{");
	for( size_t i = 0; i < st->nfields; i++ ) {
		RStructMember *member = r_struct_type_field_at_index(st, i, err);
		if( member == NULL ) goto out_struct_type_field_at_index;

		printf("\"%s\":", member->name);

		if( !
			_print_json_for_type(
				member->type,
				((uint8_t *) data) + member->offset,
				err
			)
		) goto out__print_json_for_type;

		r_struct_member_unref(member);
		if( i != st->nfields - 1 ) printf(",");
		continue;

out__print_json_for_type:
		r_struct_member_unref(member);
out_struct_type_field_at_index:
		goto out_loop_exit;
	}
	printf("}");

	r_type_unref((RType *) st);
	return true;

out_loop_exit:
	r_type_unref((RType *) st);
out_type_as_struct:
	return false;
}

static bool _print_json_for_string( RType *type, void *data, GError **err ) {
	printf("\"%s\"", *((char **) data));
	return true;
}

static bool print_function_type( RType *type, GError **err ) {
	RFunctionType *ft = r_type_as_function(type, err);
	if( ft == NULL ) return false;

	RType *rtype = r_function_return_type(ft, err);
	if( rtype == NULL ) return false; // TODO: Clean up resources

	printf("(%s (", rtype->name);

	r_type_unref(rtype);

	for( size_t i = 0; i < ft->narguments; i++ ) {
		RType *arg = r_function_argument_type_at_index(ft, i, err);
		if( arg == NULL ) return false; // TODO: Clean up resources

		printf(" %s", arg->name);
		if( i != ft->narguments - 1 ) printf(",");

		r_type_unref(arg);
	}

	printf(" ))");

	r_type_unref((RType *) ft);

	return true;
}

static bool _print_json_for_type( RType *type, void *data, GError **err ) {
	// Strings are special cased.
	if( type->id == R_TYPE_CLASS_TYPEDEF && strcmp(type->name, "string") == 0 )
		return _print_json_for_string(type, data, err);

	type = r_type_as_canonical(type, err);
	if( type == NULL ) return false;

	bool ret;
	switch( type->id ) {
		case R_TYPE_CLASS_PRIMITIVE:
			ret = _print_json_for_primitive(type, data, err);
			break;
		case R_TYPE_CLASS_STRUCT:
			ret = _print_json_for_struct(type, data, err);
			break;
		case R_TYPE_CLASS_FUNCTION:
			g_assert(false);
		case R_TYPE_CLASS_POINTER: {
			// TODO: It might be an array
			RType *pointee = r_type_pointee(type, err);
			if( pointee == NULL ) {
				ret = false;
				break;
			}
			r_type_unref(type);
			type = pointee;
			ret = _print_json_for_type(type, *((void **) data), err);
			break;
		}
		case R_TYPE_CLASS_ARRAY:
		case R_TYPE_CLASS_ENUMERATION:
		case R_TYPE_CLASS_TYPEDEF:
		case R_TYPE_CLASS_UNION:
		default:
			fprintf(stderr, "Unknown type with id %d\n", type->id);
			abort();
	}

	r_type_unref(type);
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
		},
		.an_int_ptr = &argc
	};

	print_json_for_type(f, &err);
	die_if_error(err);

	printf("\n");

	RType *type = rumination_get_type(&print_function_type, &err);
	die_if_error(err);

	RType *pointee = r_type_pointee(type, &err);
	die_if_error(err);
	r_type_unref(type);

	print_function_type(pointee, &err);
	die_if_error(err);

	r_type_unref(type);

	printf("\n");
}
