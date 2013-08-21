#include <exception>
#include <sstream>
#include <cstddef>

#include <Ice/Ice.h>
#include "ice/type.h"

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/record_member.h"
#include "ruminate/tag_type.h"
#include "ruminate/record_type.h"
#include "ruminate/function_type.h"

#define _RECORD_TYPE_CPP_

#include "private/common.h"
#include "private/type.h"
#include "private/record_member.h"
#include "private/tag_type.h"
#include "private/record_type.h"
#include "private/function_type.h"

bool r_record_type_init( RRecordType *rrt, GError **error ) RUMINATE_NOEXCEPT {
	rrt->members_init = false;

	switch( ((RType *) rrt)->type_id ) {
		case Ruminate::TypeIdStructure:
			rrt->id = R_RECORD_TYPE_STRUCTURE;
			return true;
		case Ruminate::TypeIdFunction:
			rrt->id = R_RECORD_TYPE_FUNCTION;
			return r_function_type_init((RFunctionType *) rrt, error);
		default:
			g_assert_not_reached();
	}

	g_assert_not_reached();
}

void r_record_type_destroy( RRecordType *rrt ) RUMINATE_NOEXCEPT {
	switch( rrt->id ) {
		case R_RECORD_TYPE_STRUCTURE:
			break;
		case R_RECORD_TYPE_FUNCTION:
			r_function_type_destroy((RFunctionType *) rrt);
			break;
		default:
			g_assert_not_reached();
	}

	rrt->members.clear();
	rrt->members_init = false;
}

RRecordType *r_record_type_alloc( Ruminate::TypeId id, GError **error ) RUMINATE_NOEXCEPT {
	switch( id ) {
		case Ruminate::TypeIdFunction:
			return (RRecordType *) r_function_type_alloc(id, error);
		case Ruminate::TypeIdStructure:
			return g_slice_new(RRecordType);
		default:
			g_assert_not_reached();
	}

	g_assert_not_reached();
}

void r_record_type_free( RRecordType *rrt ) RUMINATE_NOEXCEPT {
	switch( rrt->id ) {
		case R_RECORD_TYPE_STRUCTURE:
			g_slice_free(RRecordType, rrt);
			break;
		case R_RECORD_TYPE_FUNCTION:
			r_function_type_free((RFunctionType *) rrt);
			break;
		default:
			g_assert_not_reached();
	}
}

static bool init_members( RRecordType *rrt, GError **error ) RUMINATE_NOEXCEPT {
	if( !rrt->members_init ) {
		if( !gxx_call(rrt->members = ((RType *) rrt)->type->getMembers(), error) )
			return false;
		rrt->members_init = true;
	}
	return true;
}

G_BEGIN_DECLS

RRecordTypeId r_record_type_id( RRecordType *rrt, GError ** ) RUMINATE_NOEXCEPT {
	return rrt->id;
}

size_t r_record_type_nmembers( RRecordType *rrt, GError **error ) RUMINATE_NOEXCEPT {
	if( !init_members(rrt, error) ) return 0;
	return rrt->members.size();
}

RRecordMember *r_record_type_member_at( RRecordType *rrt, size_t i, GError **error ) RUMINATE_NOEXCEPT {
	if( !init_members(rrt, error) ) return 0;
	// TODO: Memoize RRecordMembers
	return r_record_member_new(rrt->members[i], error);
}

G_END_DECLS
