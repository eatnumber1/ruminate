#include <ruminate.h>

#include <glib.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <locale.h>

#define print_json_for_type(expr, err) \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wgnu-statement-expression\"") \
	({ \
		__typeof__(expr) __expr = (expr); \
		RType *_type = ruminate_get_type(__expr, err); \
		bool ret = false; \
		if( _type != NULL ) { \
			ret = _print_json_for_type(_type, &__expr, err); \
			r_type_unref(_type); \
		} \
		ret; \
	}) \
	_Pragma("clang diagnostic pop")

typedef char *string;

struct IdentifiedUnion {
	enum {
		A_LONG,
		A_CHAR
	} id;
	union {
		long a_long;
		char a_char;
	};
};

typedef struct MyStruct {
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
	struct IdentifiedUnion an_identified_union, another_identified_union;
	/*
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
	RTypeMember *tmember = (RTypeMember *) member;
	// TODO: Error checking

	r_type_member_ref(tmember);

	RType *member_type = r_type_member_type(tmember, error);
	die_if_error(*error);
	// TODO: Error checking

	RString *name = r_aggregate_member_name(member, error);
	die_if_error(*error);
	// TODO: Error checking

	printf("\"%s\":", r_string_bytes(name));

	r_string_unref(name);

	ptrdiff_t offset = r_type_member_offset(tmember, error);
	die_if_error(*error);
	// TODO: Error checking

	return _print_json_for_type(
		member_type,
		((uint8_t *) data) + offset,
		error
	);
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
					printf("\"%c\"", c);
				} else {
					printf("%hhu", (unsigned char) c);
				}
			}
			break;
		case R_BUILTIN_TYPE_BOOL:
			printf("%s", *((_Bool *) data) ? "true" : "false");
			break;
		case R_BUILTIN_TYPE_LONG:
			if( is_unsigned ) {
				printf("%lu", *((unsigned long *) data));
			} else {
				printf("%ld", *((long *) data));
			}
			break;
		default:
			printf("\"<unknowable>\"");
	}

	r_type_unref((RType *) type);
	return true;
}

static bool _print_json_for_identified_union( RAggregateType *rt, void *data, GError **error ) {
	RBuiltinTypeId union_type_id = R_BUILTIN_TYPE_UNKNOWN;

	{
		RTypeMember *struct_memb = (RTypeMember *) r_aggregate_type_member_at(rt, 0, error);
		die_if_error(*error);
		// TODO: Error checking

		RAggregateType *_enum = (RAggregateType *) r_type_member_type(struct_memb, error);
		die_if_error(*error);
		// TODO: Error checking

		r_type_member_unref(struct_memb);

		for( size_t i = 0; i < r_aggregate_type_nmembers(_enum, error); i++ ) {
			die_if_error(*error);
			// TODO: Error checking

			REnumMember *enum_memb = (REnumMember *) r_aggregate_type_member_at(_enum, i, error);
			die_if_error(*error);
			// TODO: Error checking

			intmax_t enum_memb_val = (intmax_t) r_enum_member_value_unsigned(enum_memb, error);
			die_if_error(*error);
			// TODO: Error checking

			r_type_member_unref((RTypeMember *) enum_memb);

			if( enum_memb_val != *((__typeof__(A_LONG) *) data) ) continue;

			static const RBuiltinTypeId supported_types[] = {
				[A_LONG] = R_BUILTIN_TYPE_LONG,
				[A_CHAR] = R_BUILTIN_TYPE_CHAR
			};
			union_type_id = supported_types[enum_memb_val];
			break;
		}
		die_if_error(*error);
		// TODO: Error checking

		r_type_unref((RType *) _enum);
	}

	RType *union_value_type = NULL;
	ptrdiff_t union_value_off = 0;
	{
		RTypeMember *struct_memb = (RTypeMember *) r_aggregate_type_member_at(rt, 1, error);
		die_if_error(*error);
		// TODO: Error checking

		RAggregateType *_union = (RAggregateType *) r_type_member_type(struct_memb, error);
		die_if_error(*error);
		// TODO: Error checking

		for( size_t i = 0; i < r_aggregate_type_nmembers(_union, error); i++ ) {
			die_if_error(*error);
			// TODO: Error checking

			RTypeMember *union_memb = (RTypeMember *) r_aggregate_type_member_at(_union, i, error);
			die_if_error(*error);
			// TODO: Error checking

			RBuiltinType *union_memb_type = (RBuiltinType *) r_type_member_type(union_memb, error);
			die_if_error(*error);
			// TODO: Error checking

			if( r_builtin_type_id(union_memb_type, error) == union_type_id ) {
				union_value_type = (RType *) union_memb_type;

				union_value_off = r_type_member_offset(struct_memb, error);
				die_if_error(*error);

				// TODO: Error checking
				union_value_off += r_type_member_offset(union_memb, error);
				die_if_error(*error);
			} else {
				die_if_error(*error);
				// TODO: Error checking

				r_type_unref((RType *) union_memb_type);
			}

			r_type_member_unref(union_memb);

			if( union_value_type != NULL ) break;
		}
		die_if_error(*error);
		// TODO: Error checking

		r_type_unref((RType *) _union);
		r_type_member_unref(struct_memb);

		g_assert(union_value_type != NULL);
		if( !_print_json_for_type(union_value_type, ((char *) data) + union_value_off, error) ) {
			// TODO: This leaks
			return false;
		}
	}

	r_type_unref(union_value_type);
	return true;
}

static bool _print_json_for_aggregate( RAggregateType *rt, void *data, GError **error ) {
	RAggregateTypeId id = r_aggregate_type_id(rt, error);
	die_if_error(*error);
	// TODO: Error checking

	switch( id ) {
		case R_AGGREGATE_TYPE_STRUCT: {
			RString *rs = r_type_name((RType *) rt, error);
			die_if_error(*error);
			// TODO: Error checking
			bool is_identified_union = (strcmp(r_string_bytes(rs), "IdentifiedUnion") == 0);
			r_string_unref(rs);
			if( is_identified_union )
				return _print_json_for_identified_union(rt, data, error);
			// Fallthrough
		}
		default: {
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
		}
	}

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

	r_type_unref(rtype);

	printf("%s (", r_string_bytes(rtname));

	r_string_unref(rtname);

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

		ptrdiff_t offset = r_type_member_offset(member, error);
		die_if_error(*error);
		// TODO: Error checking

		if( !_print_json_for_type(member_type, ((char *) data) + offset, error) ) {
			// TODO: This leaks
			return false;
		}
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
	r_type_ref(type);

	switch( id ) {
		case R_TYPE_BUILTIN:
			_print_json_for_builtin((RBuiltinType *) type, data, error);
			die_if_error(*error);
			// TODO: Error checking
			break;
		case R_TYPE_AGGREGATE:
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
	setbuf(stdout, NULL);

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
		.an_enum = AN_ENUM_VALUE,
		.an_identified_union = {
			.id = A_LONG,
			.a_long = 1024L
		},
		.another_identified_union = {
			.id = A_CHAR,
			.a_char = 'x'
		}
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

	{
		RString *fn = ruminate_get_function_name((void *) &print_function_type, &err);
		die_if_error(err);

		printf("%s\n", r_string_bytes(fn));

		r_string_unref(fn);
	}
}
