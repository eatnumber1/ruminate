#include <stddef.h>
#include <stdbool.h>

#include <glib.h>
#include <jansson.h>
#include <ruminate.h>

#include "json-lib.h"

#if JANSSON_MAJOR_VERSION <= 2 && JANSSON_MINOR_VERSION < 4
#define json_boolean(val) ((val) ? json_true() : json_false())
#endif

__attribute__((visibility("default")))
JsonState *json_state_new() {
	JsonState *ret = g_slice_new(JsonState);
	ret->refcnt = 1;
	g_datalist_init(&ret->handlers);
	return ret;
}

__attribute__((visibility("default")))
JsonState *json_state_ref( JsonState *js ) {
	g_atomic_int_inc(&js->refcnt);
	return js;
}

__attribute__((visibility("default")))
void json_state_unref( JsonState *js ) {
	if( g_atomic_int_dec_and_test(&js->refcnt) ) {
		g_datalist_clear(&js->handlers);
		g_slice_free(JsonState, js);
	}
}

__attribute__((visibility("default")))
void json_state_add_serializer( JsonState *js, GQuark id, JsonSerializer *ser ) {
	g_datalist_id_set_data(&js->handlers, id, ser);
}

__attribute__((visibility("default")))
void json_state_remove_serializer( JsonState *js, GQuark id ) {
	g_datalist_id_remove_data(&js->handlers, id);
}

static json_t *json_serialize_builtin( JsonState *js, RBuiltinType *rbt, void *value, GError **error ) {
	(void) js;
	RBuiltinTypeId id = r_builtin_type_id(rbt, error);
	if( id == R_BUILTIN_TYPE_UNKNOWN ) return NULL;

	switch( id ) {
		case R_BUILTIN_TYPE_INT:
			return json_integer(*((int *) value));
		case R_BUILTIN_TYPE_LONG:
			return json_integer(*((long *) value));
		case R_BUILTIN_TYPE_SHORT:
			return json_integer(*((short *) value));
		case R_BUILTIN_TYPE_CHAR:
			return json_integer(*((char *) value));
		case R_BUILTIN_TYPE_DOUBLE:
			return json_real(*((double *) value));
		case R_BUILTIN_TYPE_BOOL:
			return json_boolean(*((bool *) value));
		case R_BUILTIN_TYPE_VOID:
			// TODO: Error here
			g_assert_not_reached();
		case R_BUILTIN_TYPE_UNKNOWN:
			g_assert_not_reached();
	}

	g_assert_not_reached();
}

static bool json_serialize_struct_member( JsonState *js, RAggregateMember *ram, void *value, json_t *obj, GError **error ) {
	GError *err = NULL;

	RType *rt = r_type_member_type((RTypeMember *) ram, &err);
	if( rt == NULL ) goto error_tm_type;;

	json_t *obj_memb = json_serialize(js, rt, value, &err);
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

	if( did_insert ) {
		return obj;
	} else {
		json_decref(obj);
		return NULL;
	}

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
		if( vals_are_equal ) break;

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

	return json_integer(val);

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

		json_t *memb_json = json_serialize(js, rt, ((char *) value) + offset, &err);
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

	return array;

error_in_loop:
	json_decref(array);
error_array_size:
	g_propagate_error(error, err);
	return NULL;
}

__attribute__((visibility("default")))
json_t *json_serialize( JsonState *js, RType *rt, void *value, GError **error ) {
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
			json_t *ret = json_serialize(js, (RType *) pointee, *((void **) value), error);
			r_type_unref(pointee);
			return ret;
		}
		case R_TYPE_ARRAY:
		    return json_serialize_array(js, (RArrayType *) rt, value, error);
		case R_TYPE_TYPEDEF: {
			RType *canonical = r_typedef_type_canonical((RTypedefType *) rt, error);
			json_t *ret = json_serialize(js, canonical, value, error);
			r_type_unref(canonical);
			return ret;
		}
		default:
			// TODO: Remove this
			g_assert_not_reached();
	}

	g_assert_not_reached();
}
