// TODO: This code is very duplicated with json-lib.c. Dedup

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include <glib.h>
#include <jansson.h>
#include <ruminate.h>
#include <talloc.h>

#include "json-lib.h"

#if JANSSON_MAJOR_VERSION <= 2 && JANSSON_MINOR_VERSION < 4
#define json_boolean(val) ((val) ? json_true() : json_false())
#endif

static json_t *construct_json_from_type( RType *type, json_t *value, GError **error ) {
	RString *name = r_type_name(type, error);
	if( name == NULL ) return NULL;

	json_t *container = json_object();
	/* TODO: Check for error */
	json_object_set_new_nocheck(container, "type", json_string(r_string_bytes(name)));
	json_object_set_new_nocheck(container, "value", value);

	r_string_unref(name);
	return container;
}

#define construct_json_from_value(type, value, json_func, error) \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wgnu-statement-expression\"") \
	({ \
		json_t *ret = NULL; \
		type v = (value); \
		(void) v; \
		RType *value_type = ruminate_get_type_by_variable_name("v", error); \
		if( value_type != NULL ) { \
			ret = construct_json_from_type(value_type, (json_func(v)), error); \
			r_type_unref(value_type); \
		} \
		ret; \
	}) \
	_Pragma("clang diagnostic pop")

static json_t *json_serialize_builtin( JsonState *js, RBuiltinType *rbt, void *value, GError **error ) {
	(void) js;

	RBuiltinTypeId id = r_builtin_type_id(rbt, error);
	if( id == R_BUILTIN_TYPE_UNKNOWN ) return NULL;

	switch( id ) {
		case R_BUILTIN_TYPE_INT:
			return construct_json_from_value(int, *((int *) value), json_integer, error);
		case R_BUILTIN_TYPE_LONG:
			return construct_json_from_value(long, *((long *) value), json_integer, error);
		case R_BUILTIN_TYPE_SHORT:
			return construct_json_from_value(short, *((short *) value), json_integer, error);
		case R_BUILTIN_TYPE_CHAR:
			return construct_json_from_value(char, *((char *) value), json_integer, error);
		case R_BUILTIN_TYPE_DOUBLE:
			return construct_json_from_value(double, *((double *) value), json_real, error);
		case R_BUILTIN_TYPE_BOOL:
			return construct_json_from_value(bool, *((bool *) value), json_boolean, error);
		case R_BUILTIN_TYPE_VOID:
			// TODO: Error here
			g_assert_not_reached();
		case R_BUILTIN_TYPE_UNKNOWN:
			g_assert_not_reached();
	}
}

static bool json_serialize_struct_member( JsonState *js, RAggregateMember *ram, void *value, json_t *obj, GError **error ) {
	GError *err = NULL;

	RType *rt = r_type_member_type((RTypeMember *) ram, &err);
	if( rt == NULL ) goto error_tm_type;

	json_t *obj_memb = json_serialize_bijective(js, rt, value, &err);
	if( err != NULL ) goto error_js_ser;
	if( obj_memb == NULL ) {
		r_type_unref(rt);
		return false;
	}

	RString *name = r_aggregate_member_name(ram, &err);
	if( name == NULL ) goto error_agg_memb_name;

	if( json_object_set(obj, r_string_bytes(name), obj_memb) != 0 ) {
		g_assert_not_reached();
		// TODO: Set an error
		goto error_js_obj_set;
	}

	r_string_unref(name);
	r_type_unref(rt);

	return true;

error_js_obj_set:
	r_string_unref(name);
error_agg_memb_name:
	json_decref(obj_memb);
error_js_ser:
	r_type_unref(rt);
error_tm_type:
	g_propagate_error(error, err);
	return false;
}

static json_t *json_serialize_struct( JsonState *js, RAggregateType *rat, void *value, GError **error ) {
	GError *err = NULL;
	// TODO: Error check
	json_t *obj = json_object();
	bool did_insert = false;

	size_t members = r_aggregate_type_nmembers(rat, &err);
	if( err != NULL ) goto error_agg_nmembers;

	for( size_t i = 0; i < members; i++ ) {
		RAggregateMember *memb = r_aggregate_type_member_at(rat, i, &err);
		if( memb == NULL ) goto error_agg_memb_at;

		ptrdiff_t offset = r_type_member_offset((RTypeMember *) memb, &err);
		if( err != NULL ) goto error_tm_offset;

		did_insert |= json_serialize_struct_member(js, memb, ((char *) value) + offset, obj, &err);
		if( err != NULL ) goto error_ser_struct_memb;

		r_type_member_unref((RTypeMember *) memb);

		continue;
error_ser_struct_memb:
error_tm_offset:
		r_type_member_unref((RTypeMember *) memb);
error_agg_memb_at:
		goto error_in_loop;
	}

	if( !did_insert ) return NULL;

	json_t *container = construct_json_from_type((RType *) rat, obj, error);
	if( container == NULL ) goto error_cons_js;

	return container;
error_cons_js:
error_in_loop:
error_agg_nmembers:
		json_decref(obj);
		g_propagate_error(error, err);
		return NULL;
}

static json_t *json_serialize_enum( JsonState *js, RAggregateType *rat, void *value, GError **error ) {
	(void) js;
	GError *err = NULL;

	size_t members = r_aggregate_type_nmembers(rat, &err);
	if( err != NULL ) goto error_agg_nmembers;

	json_int_t val;
	RString *name = NULL;

	size_t i;
	for( i = 0; i < members; i++ ) {
		REnumMember *memb = (REnumMember *) r_aggregate_type_member_at(rat, i, &err);
		if( memb == NULL ) goto error_agg_memb_at;

		RBuiltinType *rbt = (RBuiltinType *) r_type_member_type((RTypeMember *) memb, &err);
		if( rbt == NULL ) goto error_tm_type;
		
		RBuiltinTypeId id = r_builtin_type_id(rbt, error);
		if( id == R_BUILTIN_TYPE_UNKNOWN ) goto error_bt_id;

		switch( id ) {
			case R_BUILTIN_TYPE_INT:
				val = *((int *) value);
				break;
			case R_BUILTIN_TYPE_LONG:
				val = *((long *) value);
				break;
			case R_BUILTIN_TYPE_SHORT:
				val = *((short *) value);
				break;
			case R_BUILTIN_TYPE_CHAR:
				val = *((char *) value);
				break;
			case R_BUILTIN_TYPE_DOUBLE:
				val = *((double *) value);
				break;
			case R_BUILTIN_TYPE_BOOL:
				val = *((bool *) value);
				break;
			case R_BUILTIN_TYPE_VOID:
			case R_BUILTIN_TYPE_UNKNOWN:
				g_assert_not_reached();
				g_assert_not_reached();
		}

		bool vals_are_equal = false;

		bool is_signed = r_builtin_type_is_signed(rbt, &err);
		if( err != NULL ) goto error_bt_is_signed;

		if( is_signed ) {
			intmax_t real_val = r_enum_member_value_signed(memb, &err);
			if( err != NULL ) goto error_ememb_val_signed;
			
			if( val == real_val ) vals_are_equal = true;
		} else {
			uintmax_t real_val = r_enum_member_value_unsigned(memb, &err);
			if( err != NULL ) goto error_ememb_val_unsigned;
			
			if( (uintmax_t) val == real_val ) vals_are_equal = true;
		}
		if( vals_are_equal ) {
			name = r_aggregate_member_name((RAggregateMember *) memb, &err);
			if( name == NULL ) {
				// TODO: Error check
				g_assert_not_reached();
			}
			break;
		}

		r_type_unref((RType *) rbt);
		r_type_member_unref((RTypeMember *) memb);

		continue;
error_ememb_val_signed:
error_ememb_val_unsigned:
error_bt_is_signed:
error_bt_id:
		r_type_unref((RType *) rbt);
error_tm_type:
		r_type_member_unref((RTypeMember *) memb);
error_agg_memb_at:
		goto error_in_loop;
	}
	if( i == members ) {
		// TODO: Error here
		g_assert_not_reached();
	}

	json_t *container = construct_json_from_type((RType *) rat, json_integer(val), error);
	if( container == NULL ) goto error_cons_js;
	json_object_set_new_nocheck(container, "name", json_string(r_string_bytes(name)));

	return container;

error_cons_js:
error_in_loop:
error_agg_nmembers:
	g_propagate_error(error, err);
	return NULL;
}

static json_t *json_serialize_aggregate( JsonState *js, RAggregateType *rat, void *value, GError **error ) {
	RAggregateTypeId id = r_aggregate_type_id(rat, error);
	if( id == R_AGGREGATE_TYPE_UNKNOWN ) return NULL;

	switch( id ) {
		case R_AGGREGATE_TYPE_UNION:
		case R_AGGREGATE_TYPE_FUNCTION:
			// TODO: Error out
			g_assert_not_reached();
		case R_AGGREGATE_TYPE_STRUCT:
			return json_serialize_struct(js, rat, value, error);
		case R_AGGREGATE_TYPE_ENUM:
		    return json_serialize_enum(js, rat, value, error);
		case R_AGGREGATE_TYPE_UNKNOWN:
			g_assert_not_reached();
	}

	g_assert_not_reached();
}

static json_t *json_serialize_array( JsonState *js, RArrayType *rat, void *value, GError **error ) {
	GError *err = NULL;

	size_t size = r_array_type_size(rat, &err);
	if( err != NULL ) goto error_array_size;

	json_t *array = json_array();

	for( size_t i = 0; i < size; i++ ) {
		RTypeMember *tm = r_array_type_member_at(rat, i, &err);
		if( tm == NULL ) goto error_array_member_at;

		RType *rt = r_type_member_type(tm, &err);
		if( rt == NULL ) goto error_tm_type;

		ptrdiff_t offset = r_type_member_offset(tm, &err);
		if( err != NULL ) goto error_tm_offset;

		json_t *memb_json = json_serialize_bijective(js, rt, ((char *) value) + offset, &err);
		if( err != NULL ) goto error_json_serialize;
		if( memb_json == NULL )
			memb_json = json_null();

		if( json_array_append(array, memb_json) != 0 )
			goto error_array_append;
		
		r_type_unref(rt);
		r_type_member_unref(tm);

		continue;
error_array_append:
		json_decref(memb_json);
error_json_serialize:
error_tm_offset:
		r_type_unref(rt);
error_tm_type:
		r_type_member_unref(tm);
error_array_member_at:
		goto error_in_loop;
	}

	size_t array_size = r_type_size((RType *) rat, &err);
	if( err != NULL ) goto error_type_size;

	return array;

error_type_size:
error_in_loop:
	json_decref(array);
error_array_size:
	g_propagate_error(error, err);
	return NULL;
}

__attribute__((visibility("default")))
json_t *json_serialize_bijective( JsonState *js, RType *rt, void *value, GError **error ) {
	RTypeId id = r_type_id(rt, error);
	if( id == R_TYPE_UNKNOWN ) return NULL;

	if( value == NULL ) return json_null();

	RString *name = r_type_name(rt, error);
	GQuark typeid = r_string_quark(name);
	r_string_unref(name);

	if( js != NULL ) {
		JsonSerializer *serializer = g_datalist_id_get_data(&js->handlers, typeid);
		if( serializer != NULL )
			return serializer->func(js, rt, value, serializer->data, error);
	}

	switch( id ) {
		case R_TYPE_BUILTIN:
			return json_serialize_builtin(js, (RBuiltinType *) rt, value, error);
		case R_TYPE_AGGREGATE:
			return json_serialize_aggregate(js, (RAggregateType *) rt, value, error);
		case R_TYPE_POINTER: {
			RPointerType *rpt = (RPointerType *) rt;
			RType *pointee = r_pointer_type_pointee(rpt, error);
			// This is not portable
			json_t *ret = json_serialize_bijective(js, (RType *) pointee, *((void **) value), error);
			r_type_unref(pointee);
			return ret;
		}
		case R_TYPE_ARRAY:
		    return json_serialize_array(js, (RArrayType *) rt, value, error);
		case R_TYPE_TYPEDEF: {
			RType *canonical = r_typedef_type_canonical((RTypedefType *) rt, error);
			json_t *ret = json_serialize_bijective(js, canonical, value, error);
			r_type_unref(canonical);
			return ret;
		}
		default:
			// TODO: Remove this
			g_assert_not_reached();
	}

	g_assert_not_reached();
}

// deserialization

typedef struct Deserialized {
	size_t size;
	void *value;
} Deserialized;

static void *json_deserialize_string( JsonState *js, TALLOC_CTX *ctx, json_t *json, void *out, GError **error ) {
	const char *str = json_string_value(json);
	if( out == NULL ) return talloc_strdup(ctx, str);

	strcpy((char *) out, str);
	return out;
}

static void *json_deserialize_object( JsonState *js, TALLOC_CTX *ctx, json_t *json, void *out, GError **error ) {
}

static void *json_deserialize_array( JsonState *js, TALLOC_CTX *ctx, json_t *json, void *out, GError **error ) {
	json_t *ary = json_object_get(json, "value");
	// TODO: Typecheck
	size_t size = json_integer_value(json_object_get(json, "size"));

	if( out == NULL ) out = talloc_new(ctx);

	ptrdiff_t skip = 0;
	for( size_t i = 0; i < json_array_size(ary); i++ ) {
		json_t *elem = json_array_get(ary, i);
		// TODO: Check for errors
		json_deserialize_bijective_rec(js, out, json, out + skip, error);
	}
}

static void *json_deserialize_bijective_rec( JsonState *js, TALLOC_CTX *ctx, json_t *json, GError **error ) {
	switch( json_typeof(json) ) {
		case JSON_OBJECT:
			return 
		case JSON_ARRAY:
		case JSON_STRING:
			return json_deserialize_string(js, json, error);
	}
}

__attribute__((visibility("default")))
void *json_deserialize_bijective( JsonState *js, json_t *json, GError **error ) {
	return json_deserialize_bijective_rec(js, NULL, json, error);
}
