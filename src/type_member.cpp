#include <exception>
#include <sstream>
#include <cstddef>

#include <Ice/Ice.h>
#include "ice/type.h"
#include "ice/type_member.h"

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/refptr.h"
#include "ruminate/type.h"
#include "ruminate/type_member.h"
#include "ruminate/record_member.h"

#define _TYPE_MEMBER_CPP_

#include "private/common.h"
#include "private/value.h"
#include "private/string.h"
#include "private/type.h"
#include "private/type_member.h"
#include "private/record_member.h"

bool r_type_member_init( RTypeMember *tm, RValue rv, GError **error ) RUMINATE_NOEXCEPT {
	tm->refcnt = 1;
	r_ptr_ref(rv.top);
	tm->mem = rv;

	switch( tm->id ) {
		case R_TYPE_MEMBER_ARRAY:
			break;
		case R_TYPE_MEMBER_RECORD:
			if( !r_record_member_init((RRecordMember *) tm, error) )
				goto err_child_init;
			break;
		default:
			g_assert_not_reached();
	}

	return true;

err_child_init:
	r_ptr_unref(rv.top);
	return false;
}

void r_type_member_destroy( RTypeMember *tm ) RUMINATE_NOEXCEPT {
	switch( tm->id ) {
		case R_TYPE_MEMBER_RECORD:
			r_record_member_destroy((RRecordMember *) tm);
		case R_TYPE_MEMBER_ARRAY:
			break;
		default:
			g_assert_not_reached();
	}
	r_ptr_unref(tm->mem.top);
	tm->mem.top = NULL;
	tm->mem.cur = NULL;
}

RTypeMember *r_type_member_alloc( RTypeMemberId id, GError **error ) RUMINATE_NOEXCEPT {
	switch( id ) {
		case R_TYPE_MEMBER_RECORD:
			return (RTypeMember *) r_record_member_alloc(error);
		case R_TYPE_MEMBER_ARRAY: {
			RTypeMember *ret = g_slice_new(RTypeMember);
			new (ret) RTypeMember();
			return ret;
		}
		default:
			g_assert_not_reached();
	}
}

void r_type_member_free( RTypeMember *tm ) RUMINATE_NOEXCEPT {
	switch( tm->id ) {
		case R_TYPE_MEMBER_RECORD:
			r_record_member_free((RRecordMember *) tm);
			break;
		case R_TYPE_MEMBER_ARRAY:
			tm->~RTypeMember();
			g_slice_free(RTypeMember, tm);
			break;
		default:
			g_assert_not_reached();
	}
}

RTypeMember *r_type_member_new( Ruminate::TypeMemberPrx &member, RTypeMemberId id, RValue rv, GError **error ) RUMINATE_NOEXCEPT {
	RTypeMember *tm = r_type_member_alloc(id, error);
	if( tm == NULL ) goto error_r_type_member_alloc;

	tm->member = member;
	tm->id = id;

	if( !r_type_member_init(tm, rv, error) ) goto error_r_type_member_init;
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

bool _r_type_member_offset( Ruminate::TypeMemberPrx &tmp, off_t *out, GError **error ) RUMINATE_NOEXCEPT {
	G_STATIC_ASSERT(sizeof(off_t) >= sizeof(__typeof__(tmp->getOffsetInBytes())));
	g_assert(out != NULL);
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
	Ruminate::TypePrx t;
	if( !gxx_call(t = tm->member->getType(), error) )
		return NULL;

	return r_type_new(t, tm->mem, error);
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
