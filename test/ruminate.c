#include <ruminate.h>

#include <glib.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <locale.h>

#define print_json_for_type(expr, err) ({ \
	GError **_err = (err); \
	__typeof__(expr) __expr = (expr); \
	RType *_type = ruminate_get_type(__expr, _err); \
	bool ret = _type == NULL ? false : _print_json_for_type(_type, &__expr, _err); \
	r_type_unref(_type); \
	ret; \
})

typedef char *string;

struct IdentifiedUnion {
	enum {
		A_LONG,
		A_SHORT
	} id;
	union {
		long a_long;
		short a_short;
	} value;
};

typedef struct __attribute__((packed)) MyStruct {
	short a_short;
	string a_string;
	struct {
		int an_int;
	} a_struct;
	int *an_int_ptr;
	char an_array[6];
	union {
		double a_double;
	} a_union;
	enum {
		AN_ENUM_VALUE
	} an_enum;
	/*
	struct IdentifiedUnion an_identified_union, another_identified_union;
	bool a_bitfield : 1;
	char an_array_of_unspecified_length[];
	*/
} MyTypedef;

void die_if_error( GError *err ) {
	if( err == NULL ) return;

	fprintf(stderr, "%s\n", err->message);

	exit(EXIT_FAILURE);
}

static bool _print_json_for_type( RType *type, void *data, GError **err );

static bool _print_json_for_aggregate_member( RAggregateMember *member, void *data, GError **error ) {
	GError *err = NULL;

	RTypeMember *tmember = (RTypeMember *) member;
	// TODO: Error checking

	RType *member_type = r_type_member_type(tmember, error);
	die_if_error(*error);
	// TODO: Error checking

	RAggregateMemberId id = r_aggregate_member_id(member, &err);
	if( err != NULL ) {
		g_propagate_error(error, err);
		return false;
	}
	switch( id ) {
		case R_AGGREGATE_MEMBER_ENUM: {
			REnumMember *emember = (REnumMember *) member;
			g_assert(r_type_id(member_type, NULL) == R_TYPE_BUILTIN);
			RBuiltinType *builtin_member_type = (RBuiltinType *) member_type;
			bool signd = r_builtin_type_is_signed(builtin_member_type, error);
			die_if_error(*error);
			// TODO: Error checking
			if( signd ) {
				intmax_t val = r_enum_member_value_signed(emember, error);
				die_if_error(*error);
				// TODO: Error checking
				printf("%jd", val);
			} else {
				uintmax_t val = r_enum_member_value_signed(emember, error);
				die_if_error(*error);
				// TODO: Error checking
				printf("%ju", val);
			}
		}
		default: {
			RString *name = r_aggregate_member_name(member, error);
			die_if_error(*error);
			// TODO: Error checking

			printf("\"%s\":", r_string_bytes(name));

			r_string_unref(name);

			off_t offset = r_type_member_offset(tmember, error);
			die_if_error(*error);
			// TODO: Error checking

			_print_json_for_type(
				member_type,
				((uint8_t *) data) + offset,
				error
			);
		}
	}

	return true;
}

static bool _print_json_for_builtin( RBuiltinType *type, void *data, GError **error ) {
	RBuiltinTypeId id = r_builtin_type_id(type, error);
	die_if_error(*error);
	// TODO: Error checking

	bool is_unsigned = r_builtin_type_is_unsigned(type, error);
	die_if_error(*error);
	// TODO: Errors
	bool is_signed = r_builtin_type_is_signed(type, error);
	die_if_error(*error);
	// TODO: Errors
	switch( id ) {
		case R_BUILTIN_TYPE_INT:
			if( is_unsigned ) {
				printf("%u", *((unsigned int *) data));
			} else {
				printf("%d", *((int *) data));
			}
			break;
		case R_BUILTIN_TYPE_SHORT:
			if( is_unsigned ) {
				printf("%hu", *((unsigned short *) data));
			} else {
				printf("%hd", *((short *) data));
			}
			break;
		case R_BUILTIN_TYPE_CHAR:
			if( is_unsigned ) {
				printf("%hhu", *((unsigned char *) data));
			} else if( is_signed ) {
				printf("%hhd", *((signed char *) data));
			} else {
				char c = *((char *) data);
				if( isalnum(c) ) {
					printf("'%c'", c);
				} else {
					printf("0x%x", c);
				}
			}
			break;
		case R_BUILTIN_TYPE_BOOL:
			printf("%s", *((_Bool *) data) ? "true" : "false");
			break;
		default:
			printf("\"unknown primitive type\"");
	}

	r_type_unref((RType *) type);
	return true;
}

static bool _print_json_for_aggregate( RAggregateType *rt, void *data, GError **error ) {
	RAggregateTypeId id = r_aggregate_type_id(rt, error);
	die_if_error(*error);
	// TODO: Error checking

	switch( id ) {
		case R_AGGREGATE_TYPE_TAG:
		case R_AGGREGATE_TYPE_FUNCTION:
			break;
		default:
			g_assert_not_reached();
	}

	size_t nmembers = r_aggregate_type_nmembers(rt, error);
	die_if_error(*error);
	// TODO: Error checking

	printf("{");
	for( size_t i = 0; i < nmembers; i++ ) {
		RAggregateMember *member = r_aggregate_type_member_at(rt, i, error);
		die_if_error(*error);
		// TODO: Error checking

		_print_json_for_aggregate_member(member, data, error);
		die_if_error(*error);
		// TODO: Error checking

		r_type_member_unref((RTypeMember *) member);
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
	RAggregateType *rrt = (RAggregateType *) rft;
	RType *rtype = r_function_type_return_type(rft, error);
	die_if_error(*error);
	// TODO: Error checking

	RString *rtname = r_type_name(rtype, error);
	die_if_error(*error);
	// TODO: Error checking

	RString *rfname = r_function_type_name(rft, error);
	die_if_error(*error);

	r_type_unref(rtype);

	printf("%s %s(", r_string_bytes(rtname), r_string_bytes(rfname));

	r_string_unref(rtname);
	r_string_unref(rfname);

	size_t narguments = r_aggregate_type_nmembers(rrt, error);
	die_if_error(*error);
	// TODO: Error checking

	for( size_t i = 0; i < narguments; i++ ) {
		RAggregateMember *arg = r_aggregate_type_member_at(rrt, i, error);
		die_if_error(*error);
		// TODO: Error checking

		RType *arg_type = r_type_member_type((RTypeMember *) arg, error);
		die_if_error(*error);

		RString *name = r_type_name(arg_type, error);
		die_if_error(*error);
		// TODO: Error checking

		r_type_unref(arg_type);

		printf(" %s", r_string_bytes(name));
		if( i != narguments - 1 ) printf(",");

		r_string_unref(name);
		r_type_member_unref((RTypeMember *) arg);
	}

	printf(" )");

	r_type_unref((RType *) rft);

	return true;
}

static bool _print_json_for_array( RArrayType *type, void *data, GError **error ) {
	size_t size = r_array_type_size(type, error);
	die_if_error(*error);
	// TODO: Error checking

	printf("[");
	for( size_t i = 0; i < size; i++ ) {
		RTypeMember *member = r_array_type_member_at(type, i, error);
		die_if_error(*error);
		// TODO: Error checking

		RType *member_type = r_type_member_type(member, error);
		die_if_error(*error);
		// TODO: Error checking

		_print_json_for_type(member_type, &data[i], error);
		if( i < size - 1 )
			printf(", ");

		r_type_unref(member_type);
		r_type_member_unref(member);
	}
	printf("]");

	return true;
}

static bool _print_json_for_type( RType *type, void *data, GError **error ) {
	RTypeId id = r_type_id(type, error);
	// TODO: Error checking

	if( id == R_TYPE_TYPEDEF ) {
		RString *name = r_type_name(type, error);
		die_if_error(*error);
		// TODO: Error checking

		// Strings are special cased.
		int cmp = strcmp(r_string_bytes(name), "string");
		r_string_unref(name);

		if( cmp == 0 )
			return _print_json_for_string(type, data, error);

		type = r_typedef_type_canonical((RTypedefType *) type, error);
		die_if_error(*error);
		// TODO: Error checking

		id = r_type_id(type, error);
		die_if_error(*error);
		// TODO: Error checking
	} else {
		r_type_ref(type);
	}

	switch( id ) {
		case R_TYPE_BUILTIN:
			_print_json_for_builtin((RBuiltinType *) type, data, error);
			die_if_error(*error);
			// TODO: Error checking
			break;
		case R_TYPE_AGGREGATE:
			// TODO: Support other tag types.
			_print_json_for_aggregate((RAggregateType *) type, data, error);
			die_if_error(*error);
			// TODO: Error checking
			break;
		case R_TYPE_POINTER: {
			RType *pointee = r_pointer_type_pointee((RPointerType *) type, error);
			die_if_error(*error);
			// TODO: Error checking
			r_type_unref(type);
			type = pointee;
			_print_json_for_type(type, *((void **) data), error);
			die_if_error(*error);
			// TODO: Error checking
			break;
		}
		case R_TYPE_ARRAY:
			_print_json_for_array((RArrayType *) type, data, error);
			die_if_error(*error);
			// TODO: Error checking
			break;
		default:
			fprintf(stderr, "Unknown type with id %d\n", id);
			abort();
	}

	r_type_unref(type);
	return true;
}

int main( int argc, char *argv[] ) {
	setlocale(LC_ALL, "");

	GError *err = NULL;
	ruminate_init(&argc, argv, &err);
	die_if_error(err);

	MyTypedef f = {
		.a_short = 1,
		.a_string = "hello",
		.a_struct = {
			.an_int = 100
		},
		.an_int_ptr = &argc,
		.an_array = "hello",
		.a_union = {
			.a_double = 3.14
		},
		.an_enum = AN_ENUM_VALUE
		/*
		.an_identified_union = {
			.id = A_LONG,
			.value = {
				.a_long = 1024L
			}
		},
		.another_identified_union = {
			.id = A_SHORT,
			.value = {
				.a_short = 255
			}
		}
		*/
	};

	{
		(void) print_json_for_type(f, &err);
		die_if_error(err);

		printf("\n");
	}

	{
		// TODO: Type check
		RPointerType *pt = (RPointerType *) ruminate_get_type(&print_function_type, &err);
		die_if_error(err);

		RType *type = r_pointer_type_pointee(pt, &err);
		die_if_error(err);
		r_type_unref((RType *) pt);

		print_function_type((RFunctionType *) type, &err);
		die_if_error(err);

		r_type_unref(type);

		printf("\n");
	}
}
