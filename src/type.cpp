#include <exception>
#include <sstream>
#include <string>
#include <cstddef>

#include <Ice/Ice.h>
#include "ice/type.h"
#include "ice/type_member.h"

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/tag_type.h"
#include "ruminate/builtin_type.h"
#include "ruminate/pointer_type.h"
#include "ruminate/typedef_type.h"

#define _TYPE_CPP_

#include "private/common.h"
#include "private/type.h"
#include "private/tag_type.h"
#include "private/builtin_type.h"
#include "private/pointer_type.h"
#include "private/typedef_type.h"
#include "private/string.h"

#if 0
template gxx_call_proto(Ruminate::TypeId);
template gxx_call_proto(Ruminate::TypePrx);
#endif

bool r_type_init( RType *rt, GError **error ) RUMINATE_NOEXCEPT {
	rt->refcnt = 1;
	rt->name = NULL;

	switch( rt->type_id ) {
		case Ruminate::TypeIdStructure:
			rt->id = R_TYPE_TAG;
			break;
		case Ruminate::TypeIdInt:
		case Ruminate::TypeIdLong:
		case Ruminate::TypeIdDouble:
		case Ruminate::TypeIdVoid:
			rt->id = R_TYPE_BUILTIN;
			break;
		case Ruminate::TypeIdTypedef:
			rt->id = R_TYPE_TYPEDEF;
		case Ruminate::TypeIdPointer:
			rt->id = R_TYPE_POINTER;
		default:
			g_assert_not_reached();
	}

	bool ret = true;
	switch( rt->id ) {
		case R_TYPE_TAG:
			ret = r_tag_type_init((RTagType *) rt, error);
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
		default:
			g_assert_not_reached();
	}

	return ret;
}

void r_type_destroy( RType *rt ) RUMINATE_NOEXCEPT {
	switch( rt->id ) {
		case R_TYPE_TAG:
			r_tag_type_destroy((RTagType *) rt);
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
		default:
			g_assert_not_reached();
	}

	if( rt->name != NULL ) r_string_unref(rt->name);
	rt->type = 0;
}


RType *r_type_alloc( Ruminate::TypeId id, GError **error ) RUMINATE_NOEXCEPT {
	switch( id ) {
		case Ruminate::TypeIdStructure:
			return (RType *) r_tag_type_alloc(id, error);
		case Ruminate::TypeIdInt:
		case Ruminate::TypeIdLong:
		case Ruminate::TypeIdDouble:
		case Ruminate::TypeIdVoid:
			return (RType *) r_builtin_type_alloc(id, error);
		case Ruminate::TypeIdTypedef:
			return (RType *) r_typedef_type_alloc(id, error);
		case Ruminate::TypeIdPointer:
			return (RType *) r_pointer_type_alloc(id, error);
		default:
			g_assert_not_reached();
	}
}

void r_type_free( RType *rt ) RUMINATE_NOEXCEPT {
	switch( rt->type_id ) {
		case Ruminate::TypeIdStructure:
			r_tag_type_free((RTagType *) rt);
			break;
		case Ruminate::TypeIdInt:
		case Ruminate::TypeIdLong:
		case Ruminate::TypeIdDouble:
		case Ruminate::TypeIdVoid:
			r_builtin_type_free((RBuiltinType *) rt);
			break;
		case Ruminate::TypeIdTypedef:
			r_typedef_type_free((RTypedefType *) rt);
			break;
		case Ruminate::TypeIdPointer:
			r_pointer_type_free((RPointerType *) rt);
			break;
		default:
			g_assert_not_reached();
	}
}

RType *r_type_new( Ruminate::TypePrx &type, GError **error ) RUMINATE_NOEXCEPT {
	RType *rt;
	Ruminate::TypeId id;

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
		if( !gxx_call(name = rt->type->getName(),  error) )
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
	Ruminate::TypePrx t;
	if( !gxx_call(t = rt->type->getPointerType(), error) )
		return NULL;
	return r_type_new(t, error);
}

G_END_DECLS
