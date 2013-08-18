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
#include "ruminate/record_member.h"

#define _RECORD_MEMBER_CPP_

#include "private/common.h"
#include "private/string.h"
#include "private/type.h"
#include "private/record_member.h"

bool r_record_member_init( RRecordMember *rm, Ruminate::TypeMemberPrx &member, GError **error ) noexcept {
	bool bitfield;

	if( !gxx_call<bool>([&member](){ return member->isBitfield(); }, &bitfield, error) )
		return false;

	rm->id = bitfield ? R_RECORD_MEMBER_BITFIELD : R_RECORD_MEMBER_OTHER;
	rm->refcnt = 1;
	rm->member = member;
	rm->name = NULL;

	return true;
}

void r_record_member_destroy( RRecordMember *rm ) noexcept {
	if( rm->name != NULL ) r_string_unref(rm->name);
	rm->member = 0;
}

RRecordMember *r_record_member_alloc( Ruminate::TypeMemberPrx &, GError ** ) noexcept {
	return g_slice_new(RRecordMember);
}

void r_record_member_free( RRecordMember *rm ) noexcept {
	g_slice_free(RRecordMember, rm);
}

RRecordMember *r_record_member_new( Ruminate::TypeMemberPrx &member, GError **error ) noexcept {
	RRecordMember *rm = r_record_member_alloc(member, error);
	if( rm == NULL ) goto error_r_record_member_alloc;

	if( !r_record_member_init(rm, member, error) ) goto error_r_record_member_init;
	return rm;

error_r_record_member_init:
	r_record_member_free(rm);
error_r_record_member_alloc:
	return NULL;
}

G_BEGIN_DECLS

RRecordMemberId r_record_member_id( RRecordMember *rm, GError ** ) noexcept {
	return rm->id;
}

RString *r_record_member_name( RRecordMember *rm, GError **error ) noexcept {
	if( rm->name == NULL ) {
		std::string name;
		if( !gxx_call([rm, &name](){ name = rm->member->getName(); }, error) )
			return NULL;

		rm->name = r_string_new(name.c_str());
	}

	return r_string_ref(rm->name);
}

RType *r_record_member_type( RRecordMember *rm, GError **error ) noexcept {
	Ruminate::TypePrx t;
	if( !gxx_call([rm, &t](){ t = rm->member->getType(); }, error) )
		return NULL;

	return r_type_new(t, error);
}

off_t r_record_member_offset( RRecordMember *rm, GError **error ) noexcept {
	G_STATIC_ASSERT(sizeof(off_t) >= sizeof(decltype(rm->member->getOffsetInBytes())));
	off_t off = 0;
	gxx_call([rm, &off](){ off = rm->member->getOffsetInBytes(); }, error);
	return off;
}

RRecordMember *r_record_member_ref( RRecordMember *rm ) noexcept {
	g_atomic_int_inc(&rm->refcnt);
	return rm;
}

void r_record_member_unref( RRecordMember *rm ) noexcept {
	if( g_atomic_int_dec_and_test(&rm->refcnt) )
		r_record_member_destroy(rm);
}

G_END_DECLS
