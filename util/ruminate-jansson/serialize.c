#include <stddef.h>
#include <stdbool.h>

#include <glib.h>
#include <jansson.h>
#include <ruminate.h>

#include "ruminate-jansson.h"
#include "common.h"

static json_t *_json_serialize( JsonState *js, RType *rt, void *value, bool, GError **error );

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
			if( js->flags & JSON_FLAG_SKIP_UNKNOWN ) return NULL;
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

	json_t *obj_memb = _json_serialize(js, rt, value, true, &err);
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
				if( js->flags & JSON_FLAG_SKIP_UNKNOWN ) return NULL;
				g_assert_not_reached();
			case R_BUILTIN_TYPE_UNKNOWN:
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
			if( js->flags & JSON_FLAG_SKIP_UNKNOWN ) return NULL;
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

		json_t *memb_json = _json_serialize(js, rt, ((char *) value) + offset, true, &err);
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

static json_t *_json_serialize_cont( JsonState *js, RType *rt, void *value, GError **error );

static json_t *_json_serialize( JsonState *js, RType *rt, void *value, bool do_hook, GError **error ) {
	RTypeId id = r_type_id(rt, error);
	if( id == R_TYPE_UNKNOWN ) return NULL;

	if( value == NULL ) {
		// TODO: Set error
		return NULL;
	}

	RString *name = r_type_name(rt, error);
	GQuark typeid = r_string_quark(name);
	r_string_unref(name);

	json_t *ret = NULL;

	if( do_hook && js != NULL ) {
		JsonHook *hook = g_datalist_id_get_data(&js->handlers, typeid);
		if( hook != NULL && hook->serializer != NULL ) {
			ret = hook->serializer((JsonSerializerArgs){ js, rt, value, _json_serialize_cont }, hook->serializer_data, error);
		}
	}

	if( ret == NULL ) {
		switch( id ) {
			case R_TYPE_BUILTIN:
				ret = json_serialize_builtin(js, (RBuiltinType *) rt, value, error);
				break;
			case R_TYPE_AGGREGATE:
				ret = json_serialize_aggregate(js, (RAggregateType *) rt, value, error);
				break;
			case R_TYPE_POINTER: {
				// TODO: This is not portable
				void *pointee_value = *((void **) value);
				if( pointee_value == NULL ) {
					ret = json_null();
				} else {
					RPointerType *rpt = (RPointerType *) rt;
					RType *pointee = r_pointer_type_pointee(rpt, error);
					ret = _json_serialize(js, (RType *) pointee, pointee_value, true, error);
					r_type_unref(pointee);
				}
				break;
			}
			case R_TYPE_ARRAY:
				ret = json_serialize_array(js, (RArrayType *) rt, value, error);
				break;
			case R_TYPE_TYPEDEF: {
				RType *canonical = r_typedef_type_canonical((RTypedefType *) rt, error);
				ret = _json_serialize(js, canonical, value, true, error);
				r_type_unref(canonical);
				break;
			}
			case R_TYPE_UNKNOWN:
				g_assert_not_reached();
		}
	}

	return ret;
}

static json_t *_json_serialize_cont( JsonState *js, RType *rt, void *value, GError **error ) {
	return _json_serialize(js, rt, value, false, error);
}

json_t *json_serialize( JsonState *js, RType *rt, void *value, GError **error ) {
	json_t *serialized = _json_serialize(js, rt, value, true, error);
	if( serialized == NULL ) return NULL;

	if( js->flags & JSON_FLAG_INVERTABLE ) {
		RString *name = r_type_name(rt, error);
		if( name == NULL ) {
			json_decref(serialized);
			return NULL;
		}
		// TODO: Error check
		json_t *obj = json_object();
		json_object_set(obj, "type", json_string(r_string_bytes(name)));
		json_object_set(obj, "value", serialized);
		r_string_unref(name);

		return obj;
	} else {
		return serialized;
	}
}
