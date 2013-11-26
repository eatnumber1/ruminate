#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include <glib.h>
#include <jansson.h>
#include <ruminate.h>

#include "ruminate-jansson.h"
#include "common.h"

static void *_json_deserialize( JsonState *js, RType *rt, json_t *json, bool, GError **error );

static void *json_deserialize_builtin( JsonState *js, RBuiltinType *rbt, json_t *json, GError **error ) {
	(void) js;
	RBuiltinTypeId id = r_builtin_type_id(rbt, error);
	if( id == R_BUILTIN_TYPE_UNKNOWN ) return NULL;

	switch( id ) {
		case R_BUILTIN_TYPE_INT: {
			int *val = r_mem_malloc(int, error);
			if( val == NULL ) return NULL;
			*val = json_integer_value(json);
			return val;
		}
		case R_BUILTIN_TYPE_LONG: {
			long *val = r_mem_malloc(long, error);
			if( val == NULL ) return NULL;
			*val = json_integer_value(json);
			return val;
		}
		case R_BUILTIN_TYPE_SHORT: {
			short *val = r_mem_malloc(short, error);
			if( val == NULL ) return NULL;
			*val = json_integer_value(json);
			return val;
		}
		case R_BUILTIN_TYPE_CHAR: {
			char *val = r_mem_malloc(char, error);
			if( val == NULL ) return NULL;
			*val = json_integer_value(json);
			return val;
		}
		case R_BUILTIN_TYPE_DOUBLE: {
			double *val = r_mem_malloc(double, error);
			if( val == NULL ) return NULL;
			*val = json_real_value(json);
			return val;
		}
		case R_BUILTIN_TYPE_BOOL: {
			bool *val = r_mem_malloc(bool, error);
			if( val == NULL ) return NULL;
			*val = json_is_true(json);
			return val;
		}
		case R_BUILTIN_TYPE_VOID:
			// TODO: Error here
			g_assert_not_reached();
		case R_BUILTIN_TYPE_UNKNOWN:
			g_assert_not_reached();
	}

	g_assert_not_reached();
}

static bool json_deserialize_struct_member( JsonState *js, RAggregateMember *ram, json_t *value, void *out, GError **error ) {
	GError *err = NULL;

	RType *rt = r_type_member_type((RTypeMember *) ram, &err);
	if( rt == NULL ) goto error_tm_type;

	void *memb = _json_deserialize(js, rt, value, true, &err);
	if( err != NULL ) goto error_js_ser;

	size_t size = r_type_size(rt, &err);
	if( err != NULL ) goto error_type_size;

	memcpy(out, memb, size);

	r_mem_unref(memb);
	r_type_unref(rt);

	return true;

error_type_size:
	r_mem_unref(memb);
error_js_ser:
	r_type_unref(rt);
error_tm_type:
	g_propagate_error(error, err);
	return false;
}

static void *json_deserialize_struct( JsonState *js, RAggregateType *rat, json_t *value, GError **error ) {
	GError *err = NULL;

	void *st = r_mem_malloc_fn((RType *) rat, &err);
	if( err != NULL ) goto error_mem_malloc;

	size_t members = r_aggregate_type_nmembers(rat, &err);
	if( err != NULL ) goto error_agg_nmembers;

	for( size_t i = 0; i < members; i++ ) {
		RAggregateMember *memb = r_aggregate_type_member_at(rat, i, &err);
		if( memb == NULL ) goto error_agg_memb_at;

		ptrdiff_t offset = r_type_member_offset((RTypeMember *) memb, &err);
		if( err != NULL ) goto error_tm_offset;

		RString *memb_name = r_aggregate_member_name(memb, &err);
		if( memb_name == NULL ) goto error_agg_memb_name;

		json_t *memb_value = json_object_get(value, r_string_bytes(memb_name));

		json_deserialize_struct_member(js, memb, memb_value, ((char *) st) + offset, &err);
		if( err != NULL ) goto error_deser_struct_memb;

		r_type_member_unref((RTypeMember *) memb);
		r_string_unref(memb_name);

		continue;
error_deser_struct_memb:
		r_string_unref(memb_name);
error_agg_memb_name:
error_tm_offset:
		r_type_member_unref((RTypeMember *) memb);
error_agg_memb_at:
		goto error_in_loop;
	}

	return st;

error_in_loop:
error_agg_nmembers:
	r_mem_unref(st);
error_mem_malloc:
	g_propagate_error(error, err);
	return NULL;
}

static void *json_deserialize_enum( JsonState *js, RAggregateType *rat, json_t *value, GError **error ) {
	(void) js;
	void *enm = r_mem_malloc_fn((RType *) rat, error);
	if( enm == NULL ) return NULL;

	*((uintmax_t *) enm) = json_integer_value(value);
	return enm;
}

static void *json_deserialize_aggregate( JsonState *js, RAggregateType *rat, json_t *value, GError **error ) {
	RAggregateTypeId id = r_aggregate_type_id(rat, error);
	if( id == R_AGGREGATE_TYPE_UNKNOWN ) return NULL;

	switch( id ) {
		case R_AGGREGATE_TYPE_UNION:
		case R_AGGREGATE_TYPE_FUNCTION:
			// TODO: Error out
			g_assert_not_reached();
		case R_AGGREGATE_TYPE_STRUCT:
			return json_deserialize_struct(js, rat, value, error);
		case R_AGGREGATE_TYPE_ENUM:
		    return json_deserialize_enum(js, rat, value, error);
		case R_AGGREGATE_TYPE_UNKNOWN:
			g_assert_not_reached();
	}

	g_assert_not_reached();
}

static void *json_deserialize_array( JsonState *js, RArrayType *rat, json_t *value, GError **error ) {
	GError *err = NULL;

	size_t size = r_array_type_size(rat, &err);
	if( err != NULL ) goto error_array_size;

	size_t type_size = r_type_size((RType *) rat, &err);
	g_assert(err == NULL);

	void *array = r_mem_malloc_sized_fn((RType *) rat, size * type_size, &err);
	g_assert(err == NULL);

	for( size_t i = 0; i < size; i++ ) {
		RTypeMember *tm = r_array_type_member_at(rat, i, &err);
		if( tm == NULL ) goto error_array_member_at;

		RType *rt = r_type_member_type(tm, &err);
		if( rt == NULL ) goto error_tm_type;

		size_t rt_size = r_type_size(rt, &err);
		g_assert(err == NULL);

		ptrdiff_t offset = r_type_member_offset(tm, &err);
		if( err != NULL ) goto error_tm_offset;

		json_t *memb_json = json_array_get(value, i);
		g_assert(memb_json != NULL);

		void *memb = _json_deserialize(js, rt, memb_json, true, &err);
		if( err != NULL ) goto error_json_deserialize;

		memcpy(((char *) array) + offset, memb, rt_size);

		r_mem_unref(memb);
		r_type_unref(rt);
		r_type_member_unref(tm);

		continue;
error_json_deserialize:
		json_decref(memb_json);
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

static void *_json_deserialize_cont( JsonState *js, RType *rt, json_t *json, GError **error );

static void *_json_deserialize( JsonState *js, RType *rt, json_t *json, bool do_hook, GError **error ) {
	RTypeId id = r_type_id(rt, error);
	if( id == R_TYPE_UNKNOWN ) return NULL;

	RString *name = r_type_name(rt, error);
	GQuark typeid = r_string_quark(name);
	r_string_unref(name);

	if( do_hook && js != NULL ) {
		JsonHook *hook = g_datalist_id_get_data(&js->handlers, typeid);
		if( hook != NULL && hook->deserializer != NULL ) {
			return hook->deserializer((JsonDeserializerArgs){ js, rt, json, _json_deserialize_cont }, hook->deserializer_data, error);
		}
	}

	switch( id ) {
		case R_TYPE_BUILTIN:
			return json_deserialize_builtin(js, (RBuiltinType *) rt, json, error);
		case R_TYPE_AGGREGATE:
			return json_deserialize_aggregate(js, (RAggregateType *) rt, json, error);
		case R_TYPE_POINTER: {
			if( json_is_null(json) ) {
				void **ppt = r_mem_malloc_fn(rt, error);
				if( ppt == NULL ) return NULL;
				*ppt = NULL;
				return ppt;
			}

			// TODO: Error check this
			RPointerType *rpt = (RPointerType *) rt;
			RType *pointee = r_pointer_type_pointee(rpt, error);
			// TODO: This is not portable
			void *pt = _json_deserialize(js, (RType *) pointee, json, true, error);
			r_type_unref(pointee);
			void **ppt = r_mem_malloc_fn(rt, error);
			*ppt = pt;
			return ppt;
		}
		case R_TYPE_ARRAY:
			return json_deserialize_array(js, (RArrayType *) rt, json, error);
		default:
			// TODO: Remove this
			g_assert_not_reached();
	}
}

static void *_json_deserialize_cont( JsonState *js, RType *rt, json_t *json, GError **error ) {
	return _json_deserialize(js, rt, json, false, error);
}

void *json_deserialize( JsonState *js, json_t *json, GError **error ) {
	GPtrArray *types = ruminate_get_types_by_name(json_string_value(json_object_get(json, "type")), error);
	if( types == NULL ) return false;
	// TODO: Handle multiple types of this name.
	//g_assert_cmpuint(types->len, ==, 1);
	RType *type = g_ptr_array_index(types, 0);
	void *ret = _json_deserialize(js, type, json_object_get(json, "value"), true, error);
	g_ptr_array_unref(types);
	return ret;
}
