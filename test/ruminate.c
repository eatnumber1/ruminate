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
	RType *_type = rumination_get_type(typeof(expr), _err); \
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

static bool _print_json_for_builtin( RBuiltinType *type, void *data, GError **error ) {
	RBuiltinTypeId id = r_builtin_type_id(type, error);
	// TODO: Error checking

	switch( id ) {
		case R_BUILTIN_TYPE_INT:
			// TODO: Errors
			if( r_builtin_type_is_unsigned(type, error) ) {
				printf("%u", *((unsigned int *) data));
			} else {
				printf("%d", *((int *) data));
			}
			break;
		case R_BUILTIN_TYPE_SHORT:
			if( r_builtin_type_is_unsigned(type, error) ) {
				printf("%hu", *((unsigned short *) data));
			} else {
				printf("%hd", *((short *) data));
			}
			break;
		case R_BUILTIN_TYPE_CHAR:
			if( r_builtin_type_is_unsigned(type, error) ) {
				printf("%hhu", *((unsigned char *) data));
			} else if( r_builtin_type_is_signed(type, error) ) {
				printf("%hhd", *((signed char *) data));
			} else {
				printf("%c", *((char *) data));
			}
			break;
		default:
			printf("\"unknown primitive type\"");
	}

	r_type_unref((RType *) type);
	return true;
}

static bool _print_json_for_type( RType *type, void *data, GError **err );

static bool _print_json_for_record( RRecordType *rt, void *data, GError **error ) {
	RRecordTypeId id = r_record_type_id(rt, error);
	// TODO: Error checking

	switch( id ) {
		case R_RECORD_TYPE_STRUCTURE:
			break;
		default:
			g_assert_not_reached();
	}

	size_t nmembers = r_record_type_nmembers(rt, error);
	// TODO: Error checking

	printf("{");
	for( size_t i = 0; i < nmembers; i++ ) {
		RRecordMember *member = r_record_type_member_at(rt, i, error);
		// TODO: Error checking

		RString *name = r_record_member_name(member, error);
		// TODO: Error checking

		printf("\"%s\":", r_string_bytes(name));

		r_string_unref(name);

		RType *member_type = r_record_member_type(member, error);
		// TODO: Error checking

		off_t offset = r_record_member_offset(member, error);
		// TODO: Error checking

		_print_json_for_type(
			member_type,
			((uint8_t *) data) + offset,
			error
		);

		r_record_member_unref(member);
		if( i != nmembers - 1 ) printf(",");
	}
	printf("}");

	r_type_unref((RType *) rt);
	return true;
}

static bool _print_json_for_string( RType *rt, void *data, GError **err ) {
	(void) rt, (void) err;
	printf("\"%s\"", *((char **) data));
	// TODO: Check for errors
	return true;
}

static bool print_function_type( RFunctionType *rft, GError **error ) {
	RRecordType *rrt = (RRecordType *) rft;
	RType *rtype = r_function_type_return_type(rft, error);
	// TODO: Error checking

	RString *rtname = r_type_name(rtype, error);
	// TODO: Error checking

	r_type_unref(rtype);

	printf("(%s (", r_string_bytes(rtname));

	r_string_unref(rtname);

	size_t narguments = r_record_type_nmembers(rrt, error);
	// TODO: Error checking

	for( size_t i = 0; i < narguments; i++ ) {
		RRecordMember *arg = r_record_type_member_at(rrt, i, error);
		// TODO: Error checking

		RString *name = r_record_member_name(arg, error);
		// TODO: Error checking

		printf(" %s", r_string_bytes(name));
		if( i != narguments - 1 ) printf(",");

		r_string_unref(name);
		r_record_member_unref(arg);
	}

	printf(" ))");

	r_type_unref((RType *) rft);

	return true;
}

static bool _print_json_for_type( RType *type, void *data, GError **error ) {
	RTypeId id = r_type_id(type, error);
	// TODO: Error checking

	if( id == R_TYPE_TYPEDEF ) {
		RString *name = r_type_name(type, error);
		// TODO: Error checking

		// Strings are special cased.
		int cmp = strcmp(r_string_bytes(name), "string");
		r_string_unref(name);

		if( cmp == 0 )
			return _print_json_for_string(type, data, error);

		type = r_typedef_type_canonical((RTypedefType *) type, error);
		// TODO: Error checking
	} else {
		r_type_ref(type);
	}

	switch( id ) {
		case R_TYPE_BUILTIN:
			_print_json_for_builtin((RBuiltinType *) type, data, error);
			// TODO: Error checking
			break;
		case R_TYPE_TAG:
			// TODO: Support other tag types.
			_print_json_for_record((RRecordType *) type, data, error);
			// TODO: Error checking
			break;
		case R_TYPE_POINTER: {
			// TODO: It might be an array
			RType *pointee = r_pointer_type_pointee((RPointerType *) type, error);
			// TODO: Error checking
			r_type_unref(type);
			type = pointee;
			_print_json_for_type(type, *((void **) data), error);
			// TODO: Error checking
			break;
		}
		default:
			fprintf(stderr, "Unknown type with id %d\n", id);
			abort();
	}

	r_type_unref(type);
	return true;
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

	RType *type = rumination_get_type(typeof(print_function_type), &err);
	die_if_error(err);

	print_function_type((RFunctionType *) type, &err);
	die_if_error(err);

	r_type_unref(type);

	printf("\n");
}
