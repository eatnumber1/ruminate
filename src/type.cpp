#include <exception>
#include <sstream>
#include <string>
#include <cstddef>
#include <new>

#include <Ice/Ice.h>
#include "ice/type.h"
#include "ice/type_member.h"

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
#include "private/memory.h"
#include "private/type.h"
#include "private/aggregate_type.h"
#include "private/builtin_type.h"
#include "private/pointer_type.h"
#include "private/typedef_type.h"
#include "private/array_type.h"
#include "private/string.h"

bool r_type_init( RType *rt, RMemory *rv, void *cur, GError **error ) RUMINATE_NOEXCEPT {
	rt->refcnt = 1;
	rt->name = NULL;
	rt->ptr = r_memory_ref(rv);
	rt->cur = cur;

	switch( rt->type_id ) {
		case Ruminate::TypeIdArray:
			rt->id = R_TYPE_ARRAY;
			break;
		case Ruminate::TypeIdStructure:
		case Ruminate::TypeIdUnion:
		case Ruminate::TypeIdEnum:
		case Ruminate::TypeIdFunction:
			rt->id = R_TYPE_AGGREGATE;
			break;
		case Ruminate::TypeIdInt:
		case Ruminate::TypeIdLong:
		case Ruminate::TypeIdDouble:
		case Ruminate::TypeIdVoid:
		case Ruminate::TypeIdShort:
		case Ruminate::TypeIdChar:
		case Ruminate::TypeIdBool:
			rt->id = R_TYPE_BUILTIN;
			break;
		case Ruminate::TypeIdTypedef:
			rt->id = R_TYPE_TYPEDEF;
			break;
		case Ruminate::TypeIdPointer:
			rt->id = R_TYPE_POINTER;
			break;
		case Ruminate::TypeIdUnknown:
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

	if( !ret ) r_memory_unref(rv);
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
	r_memory_unref(rt->ptr);
}


RType *r_type_alloc( Ruminate::TypeId id, GError **error ) RUMINATE_NOEXCEPT {
	switch( id ) {
		case Ruminate::TypeIdArray:
			return (RType *) r_array_type_alloc(id, error);
		case Ruminate::TypeIdStructure:
		case Ruminate::TypeIdEnum:
		case Ruminate::TypeIdUnion:
		case Ruminate::TypeIdFunction:
			return (RType *) r_aggregate_type_alloc(id, error);
		case Ruminate::TypeIdInt:
		case Ruminate::TypeIdLong:
		case Ruminate::TypeIdDouble:
		case Ruminate::TypeIdVoid:
		case Ruminate::TypeIdShort:
		case Ruminate::TypeIdChar:
		case Ruminate::TypeIdBool:
			return (RType *) r_builtin_type_alloc(id, error);
		case Ruminate::TypeIdTypedef:
			return (RType *) r_typedef_type_alloc(id, error);
		case Ruminate::TypeIdPointer:
			return (RType *) r_pointer_type_alloc(id, error);
		case Ruminate::TypeIdUnknown: {
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
		case Ruminate::TypeIdArray:
			r_array_type_free((RArrayType *) rt);
			break;
		case Ruminate::TypeIdStructure:
		case Ruminate::TypeIdUnion:
		case Ruminate::TypeIdEnum:
		case Ruminate::TypeIdFunction:
			r_aggregate_type_free((RAggregateType *) rt);
			break;
		case Ruminate::TypeIdInt:
		case Ruminate::TypeIdLong:
		case Ruminate::TypeIdDouble:
		case Ruminate::TypeIdVoid:
		case Ruminate::TypeIdShort:
		case Ruminate::TypeIdChar:
		case Ruminate::TypeIdBool:
			r_builtin_type_free((RBuiltinType *) rt);
			break;
		case Ruminate::TypeIdTypedef:
			r_typedef_type_free((RTypedefType *) rt);
			break;
		case Ruminate::TypeIdPointer:
			r_pointer_type_free((RPointerType *) rt);
			break;
		case Ruminate::TypeIdUnknown:
			rt->~RType();
			g_slice_free(RType, rt);
			break;
		default:
			g_assert_not_reached();
	}
}

RType *r_type_new( Ruminate::TypePrx &type, RMemory *rv, void *cur, GError **error ) RUMINATE_NOEXCEPT {
	RType *rt;
	Ruminate::TypeId id;

	if( !gxx_call(id = type->getId(), error) )
		goto error_getId;

	if( (rt = r_type_alloc(id, error)) == NULL ) goto error_r_type_alloc;

	rt->type = type;
	rt->type_id = id;

	if( !r_type_init(rt, rv, cur, error) ) goto error_r_type_init;

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
		if( !gxx_call(name = rt->type->getName(IceUtil::None), error) )
			return NULL;
		rt->name = r_string_new(name.c_str());
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
	// TODO: Runtime error if rt is not value-backed.
	RMemory *rm;
	Ruminate::TypePrx t;
	void **ptrptr;
	
	ptrptr = g_new(void *, 1);
	*ptrptr = rt->cur;
	rm = r_memory_new(ptrptr, rt->ptr, error);
	if( rm == NULL ) goto err_r_memory_new;

	G_STATIC_ASSERT(sizeof(void **) <= sizeof(::Ice::Long));
	if( !gxx_call(t = rt->type->getPointerType((::Ice::Long) ptrptr), error) )
		goto err_getPointerType;
	return r_type_new(t, rm, ptrptr, error);

err_getPointerType:
	r_memory_unref(rm);
err_r_memory_new:
	g_free(ptrptr);
	return NULL;
}

G_END_DECLS
