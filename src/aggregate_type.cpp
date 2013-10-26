#include <exception>
#include <sstream>
#include <cstddef>
#include <new>

#include <stdint.h>

#include <Ice/Ice.h>
#include "ice/type.h"

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/type_member.h"
#include "ruminate/aggregate_member.h"
#include "ruminate/aggregate_type.h"
#include "ruminate/tag_type.h"
#include "ruminate/function_type.h"
#include "ruminate/frame.h"
#include "ruminate/rumination.h"

#include "private/common.h"
#include "private/gettid.h"
#include "private/memory.h"
#include "private/type.h"
#include "private/type_member.h"
#include "private/aggregate_member.h"
#include "private/aggregate_type.h"
#include "private/tag_type.h"
#include "private/function_type.h"

bool r_aggregate_type_init( RAggregateType *rrt, GError **error ) RUMINATE_NOEXCEPT {
	rrt->members_init = false;

	switch( ((RType *) rrt)->type_id ) {
		case Ruminate::TypeIdStructure:
		case Ruminate::TypeIdUnion:
		case Ruminate::TypeIdEnum:
			rrt->id = R_AGGREGATE_TYPE_TAG;
			return r_tag_type_init((RTagType *) rrt, error);;
		case Ruminate::TypeIdFunction:
			rrt->id = R_AGGREGATE_TYPE_FUNCTION;
			return r_function_type_init((RFunctionType *) rrt, error);
		default:
			g_assert_not_reached();
	}

	g_assert_not_reached();
}

void r_aggregate_type_destroy( RAggregateType *rrt ) RUMINATE_NOEXCEPT {
	switch( rrt->id ) {
		case R_AGGREGATE_TYPE_TAG:
			r_tag_type_destroy((RTagType *) rrt);
			break;
		case R_AGGREGATE_TYPE_FUNCTION:
			r_function_type_destroy((RFunctionType *) rrt);
			break;
		default:
			g_assert_not_reached();
	}

	if( rrt->members_init ) {
		rrt->members.clear();
		rrt->members_init = false;
	}
}

RAggregateType *r_aggregate_type_alloc( Ruminate::TypeId id, GError **error ) RUMINATE_NOEXCEPT {
	switch( id ) {
		case Ruminate::TypeIdFunction:
			return (RAggregateType *) r_function_type_alloc(id, error);
		case Ruminate::TypeIdUnion:
		case Ruminate::TypeIdEnum:
		case Ruminate::TypeIdStructure:
			return (RAggregateType *) r_tag_type_alloc(id, error);
		default:
			g_assert_not_reached();
	}

	g_assert_not_reached();
}

void r_aggregate_type_free( RAggregateType *rrt ) RUMINATE_NOEXCEPT {
	switch( rrt->id ) {
		case R_AGGREGATE_TYPE_TAG:
			r_tag_type_free((RTagType *) rrt);
			break;
		case R_AGGREGATE_TYPE_FUNCTION:
			r_function_type_free((RFunctionType *) rrt);
			break;
		default:
			g_assert_not_reached();
	}
}

static bool init_members( RAggregateType *rrt, GError **error ) RUMINATE_NOEXCEPT {
	if( !rrt->members_init ) {
		RType *rt = (RType *) rrt;
		if( !gxx_call(rrt->members = rt->type->getMembers(IceUtil::None), error) )
			return false;
		rrt->members_init = true;
	}
	return true;
}

G_BEGIN_DECLS

RAggregateTypeId r_aggregate_type_id( RAggregateType *rrt, GError ** ) RUMINATE_NOEXCEPT {
	return rrt->id;
}

size_t r_aggregate_type_nmembers( RAggregateType *rrt, GError **error ) RUMINATE_NOEXCEPT {
	if( !init_members(rrt, error) ) return 0;
	return rrt->members.size();
}

RAggregateMember *r_aggregate_type_member_at( RAggregateType *rrt, size_t i, GError **error ) RUMINATE_NOEXCEPT {
	if( !init_members(rrt, error) ) return 0;
	// TODO: vector access could throw
	Ruminate::TypeMemberPrx tmp = rrt->members[i];
	off_t offset = 0;
	if( rrt->id != R_AGGREGATE_TYPE_FUNCTION ) {
		if( !_r_type_member_offset(tmp, &offset, error) ) return NULL;
	}
	// TODO: Memoize RAggregateMembers
	RType *rt = (RType *) rrt;
	return (RAggregateMember *) r_type_member_new(tmp, R_TYPE_MEMBER_AGGREGATE, rt->ptr, ((uint8_t *) rt->cur) + offset, error);
}

G_END_DECLS
