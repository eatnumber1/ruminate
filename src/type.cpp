#include "config.h"

#include <exception>
#include <sstream>
#include <string>
#include <cstddef>
#include <new>

#include "private/ice.h"
#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/aggregate_member.h"
#include "ruminate/aggregate_type.h"
#include "ruminate/builtin_type.h"
#include "ruminate/pointer_type.h"
#include "ruminate/typedef_type.h"
#include "ruminate/type_member.h"
#include "ruminate/array_type.h"

#include "private/common.h"
#include "private/type.h"
#include "private/aggregate_type.h"
#include "private/builtin_type.h"
#include "private/pointer_type.h"
#include "private/typedef_type.h"
#include "private/array_type.h"
#include "private/string.h"

bool r_type_init( RType *rt, GError **error ) RUMINATE_NOEXCEPT {
	rt->refcnt = 1;
	rt->name = NULL;
	rt->size.initialized = false;

	switch( rt->type_id ) {
		case RuminateBackend::TypeIdArray:
			rt->id = R_TYPE_ARRAY;
			break;
		case RuminateBackend::TypeIdStructure:
		case RuminateBackend::TypeIdUnion:
		case RuminateBackend::TypeIdEnum:
		case RuminateBackend::TypeIdFunction:
			rt->id = R_TYPE_AGGREGATE;
			break;
		case RuminateBackend::TypeIdInt:
		case RuminateBackend::TypeIdLong:
		case RuminateBackend::TypeIdDouble:
		case RuminateBackend::TypeIdVoid:
		case RuminateBackend::TypeIdShort:
		case RuminateBackend::TypeIdChar:
		case RuminateBackend::TypeIdBool:
			rt->id = R_TYPE_BUILTIN;
			break;
		case RuminateBackend::TypeIdTypedef:
			rt->id = R_TYPE_TYPEDEF;
			break;
		case RuminateBackend::TypeIdPointer:
			rt->id = R_TYPE_POINTER;
			break;
		case RuminateBackend::TypeIdUnknown:
			rt->id = R_TYPE_UNKNOWN;
			break;
		default:
			g_assert_not_reached();
	}

	bool ret = true;
	switch( rt->id ) {
		case R_TYPE_AGGREGATE:
			ret = r_aggregate_type_init((RAggregateType *) rt, error);
			break;
		case R_TYPE_BUILTIN:
			ret = r_builtin_type_init((RBuiltinType *) rt, error);
			break;
		case R_TYPE_TYPEDEF:
			ret = r_typedef_type_init((RTypedefType *) rt, error);
			break;
		case R_TYPE_POINTER:
			ret = r_pointer_type_init((RPointerType *) rt, error);
			break;
		case R_TYPE_ARRAY:
			ret = r_array_type_init((RArrayType *) rt, error);
			break;
		case R_TYPE_UNKNOWN:
			break;
		default:
			g_assert_not_reached();
	}

	return ret;
}

void r_type_destroy( RType *rt ) RUMINATE_NOEXCEPT {
	switch( rt->id ) {
		case R_TYPE_AGGREGATE:
			r_aggregate_type_destroy((RAggregateType *) rt);
			break;
		case R_TYPE_BUILTIN:
			r_builtin_type_destroy((RBuiltinType *) rt);
			break;
		case R_TYPE_TYPEDEF:
			r_typedef_type_destroy((RTypedefType *) rt);
			break;
		case R_TYPE_POINTER:
			r_pointer_type_destroy((RPointerType *) rt);
			break;
		case R_TYPE_ARRAY:
			r_array_type_destroy((RArrayType *) rt);
			break;
		case R_TYPE_UNKNOWN:
			break;
		default:
			g_assert_not_reached();
	}

	if( rt->name != NULL ) r_string_unref(rt->name);
	rt->type = 0;
}


RType *r_type_alloc( RuminateBackend::TypeId id, GError **error ) RUMINATE_NOEXCEPT {
	switch( id ) {
		case RuminateBackend::TypeIdArray:
			return (RType *) r_array_type_alloc(id, error);
		case RuminateBackend::TypeIdStructure:
		case RuminateBackend::TypeIdEnum:
		case RuminateBackend::TypeIdUnion:
		case RuminateBackend::TypeIdFunction:
			return (RType *) r_aggregate_type_alloc(id, error);
		case RuminateBackend::TypeIdInt:
		case RuminateBackend::TypeIdLong:
		case RuminateBackend::TypeIdDouble:
		case RuminateBackend::TypeIdVoid:
		case RuminateBackend::TypeIdShort:
		case RuminateBackend::TypeIdChar:
		case RuminateBackend::TypeIdBool:
			return (RType *) r_builtin_type_alloc(id, error);
		case RuminateBackend::TypeIdTypedef:
			return (RType *) r_typedef_type_alloc(id, error);
		case RuminateBackend::TypeIdPointer:
			return (RType *) r_pointer_type_alloc(id, error);
		case RuminateBackend::TypeIdUnknown: {
			RType *ret = g_slice_new(RType);
			new (ret) RType();
			return ret;
		}
		default:
			g_assert_not_reached();
	}
}

void r_type_free( RType *rt ) RUMINATE_NOEXCEPT {
	switch( rt->type_id ) {
		case RuminateBackend::TypeIdArray:
			r_array_type_free((RArrayType *) rt);
			break;
		case RuminateBackend::TypeIdStructure:
		case RuminateBackend::TypeIdUnion:
		case RuminateBackend::TypeIdEnum:
		case RuminateBackend::TypeIdFunction:
			r_aggregate_type_free((RAggregateType *) rt);
			break;
		case RuminateBackend::TypeIdInt:
		case RuminateBackend::TypeIdLong:
		case RuminateBackend::TypeIdDouble:
		case RuminateBackend::TypeIdVoid:
		case RuminateBackend::TypeIdShort:
		case RuminateBackend::TypeIdChar:
		case RuminateBackend::TypeIdBool:
			r_builtin_type_free((RBuiltinType *) rt);
			break;
		case RuminateBackend::TypeIdTypedef:
			r_typedef_type_free((RTypedefType *) rt);
			break;
		case RuminateBackend::TypeIdPointer:
			r_pointer_type_free((RPointerType *) rt);
			break;
		case RuminateBackend::TypeIdUnknown:
			rt->~RType();
			g_slice_free(RType, rt);
			break;
		default:
			g_assert_not_reached();
	}
}

RType *r_type_new( RuminateBackend::TypePrx &type, GError **error ) RUMINATE_NOEXCEPT {
	RType *rt;
	RuminateBackend::TypeId id;

	if( !gxx_call(id = type->getId(), error) )
		goto error_getId;

	if( (rt = r_type_alloc(id, error)) == NULL ) goto error_r_type_alloc;

	rt->type = type;
	rt->type_id = id;

	if( !r_type_init(rt, error) ) goto error_r_type_init;

	return rt;

error_r_type_init:
	rt->type = 0;
	r_type_free(rt);
error_r_type_alloc:
error_getId:
	return NULL;
}

void r_type_delete( RType *rt ) RUMINATE_NOEXCEPT {
	r_type_destroy(rt);
	r_type_free(rt);
}

G_BEGIN_DECLS

RTypeId r_type_id( RType *rt, GError **error ) RUMINATE_NOEXCEPT {
	(void) error;
	return rt->id;
}

RString *r_type_name( RType *rt, GError **error ) RUMINATE_NOEXCEPT {
	if( rt->name == NULL ) {
		std::string name;
		if( !gxx_call(name = rt->type->getName(), error) )
			return NULL;
		rt->name = r_string_new_cxx(name);
	}

	return r_string_ref(rt->name);
}

RType *r_type_ref( RType *rt ) RUMINATE_NOEXCEPT {
	g_atomic_int_inc(&rt->refcnt);
	return rt;
}

void r_type_unref( RType *rt ) RUMINATE_NOEXCEPT {
	if( g_atomic_int_dec_and_test(&rt->refcnt) )
		r_type_delete(rt);
}

RType *r_type_pointer( RType *rt, GError **error ) RUMINATE_NOEXCEPT {
	RuminateBackend::TypePrx t;
	
	if( !gxx_call(t = rt->type->getPointerType(), error) )
		return NULL;

	return r_type_new(t, error);
}

size_t r_type_size( RType *rt, GError **error ) RUMINATE_NOEXCEPT {
	if( rt->size.initialized ) return rt->size.value;

	if( !gxx_call(rt->size.value = (size_t) rt->type->getSize(), error) )
		return 0;
	rt->size.initialized = true;
	return rt->size.value;
}

G_END_DECLS
