#include <functional>
#include <memory>
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

#define _TYPE_CPP_

#include "private/common.h"
#include "private/type.h"
#include "private/tag_type.h"
#include "private/string.h"

template gxx_call_proto(Ruminate::TypeId);
template gxx_call_proto(Ruminate::TypePrx);

bool r_type_init( RType *rt, GError **error ) noexcept {
	rt->refcnt = 1;
	rt->name = NULL;

	switch( rt->type_id ) {
		case Ruminate::TypeIdStructure:
			rt->id = R_TYPE_TAG;
			break;
		default:
			g_assert_not_reached();
	}

	bool ret = true;
	switch( rt->id ) {
		case R_TYPE_TAG:
			ret = r_tag_type_init((RTagType *) rt, error);
			break;
		default:
			g_assert_not_reached();
	}

	return ret;
}

void r_type_destroy( RType *rt ) noexcept {
	switch( rt->id ) {
		case R_TYPE_TAG:
			r_tag_type_destroy((RTagType *) rt);
			break;
		default:
			g_assert_not_reached();
	}

	if( rt->name != NULL ) r_string_unref(rt->name);
	rt->type = 0;
}


RType *r_type_alloc( Ruminate::TypeId id, GError **error ) noexcept {
	switch( id ) {
		case Ruminate::TypeIdStructure:
			return (RType *) r_tag_type_alloc(id, error);
		default:
			g_assert_not_reached();
	}
}

void r_type_free( RType *rt ) noexcept {
	switch( rt->type_id ) {
		case Ruminate::TypeIdStructure:
			r_tag_type_free((RTagType *) rt);
		default:
			g_assert_not_reached();
	}
}

RType *r_type_new( Ruminate::TypePrx &type, GError **error ) noexcept {
	RType *rt;
	Ruminate::TypeId id;

	if( !gxx_call<Ruminate::TypeId>([&type](){ return type->getId(); }, &id, error) )
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

void r_type_delete( RType *rt ) noexcept {
	r_type_destroy(rt);
	r_type_free(rt);
}

G_BEGIN_DECLS

RTypeId r_type_id( RType *rt, GError **error ) noexcept {
	(void) error;
	return rt->id;
}

const RString *r_type_name( RType *rt, GError **error ) noexcept {
	if( rt->name == NULL ) {
		std::string name;
		if( !gxx_call<std::string>([rt](){ return rt->type->getName(); }, &name, error) )
			return NULL;
		rt->name = r_string_new(name.c_str());
	}

	return r_string_ref(rt->name);
}

RType *r_type_ref( RType *rt ) noexcept {
	g_atomic_int_inc(&rt->refcnt);
	return rt;
}

void r_type_unref( RType *rt ) noexcept {
	if( g_atomic_int_dec_and_test(&rt->refcnt) )
		r_type_delete(rt);
}

G_END_DECLS
