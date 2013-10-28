#include <exception>
#include <sstream>
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
#include "ruminate/type_member.h"
#include "ruminate/aggregate_member.h"

#include "private/common.h"
#include "private/memory.h"
#include "private/string.h"
#include "private/type.h"
#include "private/type_member.h"
#include "private/aggregate_member.h"

bool r_type_member_init( RTypeMember *tm, RType *container, RMemory *rv, void *cur, GError **error ) RUMINATE_NOEXCEPT {
	switch( container->type_id ) {
		case RuminateBackend::TypeIdArray:
			tm->id = R_TYPE_MEMBER_ARRAY;
			break;
		case RuminateBackend::TypeIdStructure:
		case RuminateBackend::TypeIdEnum:
		case RuminateBackend::TypeIdUnion:
		case RuminateBackend::TypeIdFunction:
			tm->id = R_TYPE_MEMBER_AGGREGATE;
			break;
		default:
			g_assert_not_reached();
	}

	tm->refcnt = 1;
	tm->ptr = r_memory_ref(rv);
	tm->cur = cur;
	tm->type = NULL;

	switch( tm->id ) {
		case R_TYPE_MEMBER_ARRAY:
			break;
		case R_TYPE_MEMBER_AGGREGATE:
			if( !r_aggregate_member_init((RAggregateMember *) tm, container, error) )
				goto err_child_init;
			break;
		default:
			g_assert_not_reached();
	}

	return true;

err_child_init:
	r_memory_unref(rv);
	return false;
}

void r_type_member_destroy( RTypeMember *tm ) RUMINATE_NOEXCEPT {
	switch( tm->id ) {
		case R_TYPE_MEMBER_AGGREGATE:
			r_aggregate_member_destroy((RAggregateMember *) tm);
		case R_TYPE_MEMBER_ARRAY:
			break;
		default:
			g_assert_not_reached();
	}
	if( tm->type != NULL ) r_type_unref(tm->type);
	tm->type = NULL;
	r_memory_unref(tm->ptr);
	tm->ptr = NULL;
	tm->cur = NULL;
}

RTypeMember *r_type_member_alloc( RType *container, GError **error ) RUMINATE_NOEXCEPT {
	switch( container->type_id ) {
		case RuminateBackend::TypeIdArray: {
			RTypeMember *ret = g_slice_new(RTypeMember);
			new (ret) RTypeMember();
			return ret;
		}
		case RuminateBackend::TypeIdStructure:
		case RuminateBackend::TypeIdEnum:
		case RuminateBackend::TypeIdUnion:
		case RuminateBackend::TypeIdFunction:
			return (RTypeMember *) r_aggregate_member_alloc(container, error);
		default:
			g_assert_not_reached();
	}

	g_assert_not_reached();
}

void r_type_member_free( RTypeMember *tm ) RUMINATE_NOEXCEPT {
	switch( tm->id ) {
		case R_TYPE_MEMBER_AGGREGATE:
			r_aggregate_member_free((RAggregateMember *) tm);
			break;
		case R_TYPE_MEMBER_ARRAY:
			tm->~RTypeMember();
			g_slice_free(RTypeMember, tm);
			break;
		default:
			g_assert_not_reached();
	}
}

RTypeMember *r_type_member_new( RuminateBackend::TypeMemberPrx &member, RType *container, RMemory *rv, void *cur, GError **error ) RUMINATE_NOEXCEPT {
	RTypeMember *tm = r_type_member_alloc(container, error);
	if( tm == NULL ) goto error_r_type_member_alloc;

	tm->member = member;

	if( !r_type_member_init(tm, container, rv, cur, error) ) goto error_r_type_member_init;
	return tm;

error_r_type_member_init:
	r_type_member_free(tm);
error_r_type_member_alloc:
	return NULL;
}

void r_type_member_delete( RTypeMember *tm ) RUMINATE_NOEXCEPT {
	r_type_member_destroy(tm);
	r_type_member_free(tm);
}

bool _r_type_member_offset( RuminateBackend::TypeMemberPrx &tmp, off_t *out, GError **error ) RUMINATE_NOEXCEPT {
	G_STATIC_ASSERT(sizeof(off_t) >= sizeof(__typeof__(tmp->getOffsetInBytes())));
	g_assert(out != NULL);

	// TODO: Error here if this type member has no offset (function arguments)
	off_t off = 0;
	if( !gxx_call(off = tmp->getOffsetInBytes(), error) )
		return false;
	*out = off;
	return true;
}

G_BEGIN_DECLS

RTypeMemberId r_type_member_id( RTypeMember *tm, GError ** ) RUMINATE_NOEXCEPT {
	return tm->id;
}

RType *r_type_member_type( RTypeMember *tm, GError **error ) RUMINATE_NOEXCEPT {
	if( tm->type != NULL ) return r_type_ref(tm->type);

	RuminateBackend::TypePrx t;
	if( !gxx_call(t = tm->member->getType(), error) )
		return NULL;

	tm->type = r_type_new(t, tm->ptr, tm->cur, error);
	return r_type_ref(tm->type);
}

off_t r_type_member_offset( RTypeMember *tm, GError **error ) RUMINATE_NOEXCEPT {
	off_t out = 0;
	_r_type_member_offset(tm->member, &out, error);
	return out;
}

RTypeMember *r_type_member_ref( RTypeMember *tm ) RUMINATE_NOEXCEPT {
	g_atomic_int_inc(&tm->refcnt);
	return tm;
}

void r_type_member_unref( RTypeMember *tm ) RUMINATE_NOEXCEPT {
	if( g_atomic_int_dec_and_test(&tm->refcnt) )
		r_type_member_delete(tm);
}

G_END_DECLS
