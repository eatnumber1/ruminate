#include <functional>
#include <memory>
#include <string>
#include <cstddef>

#include <Ice/Ice.h>
#include "ice/type.h"

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/record_member.h"
#include "ruminate/type.h"
#include "ruminate/tag_type.h"
#include "ruminate/record_type.h"

#define _RECORD_TYPE_CPP_

#include "private/common.h"
#include "private/record_member.h"
#include "private/type.h"
#include "private/tag_type.h"
#include "private/record_type.h"

template gxx_call_proto(Ruminate::TypeMemberList);

bool r_record_type_init( RRecordType *rrt, GError ** ) noexcept {
	rrt->members_init = false;

	switch( ((RType *) rrt)->type_id ) {
		case Ruminate::TypeIdStructure:
			rrt->id = R_RECORD_TYPE_STRUCTURE;
			break;
		default:
			g_assert_not_reached();
	}

	return true;
}

void r_record_type_destroy( RRecordType *rrt ) noexcept {
	rrt->members.clear();
	rrt->members_init = false;
}

RRecordType *r_record_type_alloc( Ruminate::TypeId, GError ** ) noexcept {
	return g_slice_new(RRecordType);
}

void r_record_type_free( RRecordType *rrt ) noexcept {
	g_slice_free(RRecordType, rrt);
}

static bool init_members( RRecordType *rrt, GError **error ) noexcept {
	if( !rrt->members_init ) {
		if( !gxx_call<Ruminate::TypeMemberList>([rrt](){ return ((RType *) rrt)->type->getMembers(); }, &rrt->members, error) )
			return false;
		rrt->members_init = true;
	}
	return true;
}

G_BEGIN_DECLS

RRecordTypeId r_record_type_id( RRecordType *rrt, GError ** ) noexcept {
	return rrt->id;
}

size_t r_record_type_nmembers( RRecordType *rrt, GError **error ) noexcept {
	if( !init_members(rrt, error) ) return 0;
	return rrt->members.size();
}

RRecordMember *r_record_type_member_at( RRecordType *rrt, size_t i, GError **error ) noexcept {
	if( !init_members(rrt, error) ) return 0;
	// TODO: Memoize RRecordMembers
	return r_record_member_new(rrt->members[i], error);
}

G_END_DECLS
