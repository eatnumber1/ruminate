#include "config.h"

#include <exception>
#include <sstream>
#include <cstddef>
#include <new>

#include <stdint.h>

#include "private/ice.h"
#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/type_member.h"
#include "ruminate/aggregate_member.h"
#include "ruminate/aggregate_type.h"
#include "ruminate/function_type.h"
#include "ruminate/frame.h"
#include "ruminate/ruminate.h"

#include "private/common.h"
#include "private/gettid.h"
#include "private/type.h"
#include "private/type_member.h"
#include "private/aggregate_member.h"
#include "private/aggregate_type.h"
#include "private/function_type.h"

bool r_aggregate_type_init( RAggregateType *rat, GError **error ) RUMINATE_NOEXCEPT {
	rat->members_init = false;

	switch( ((RType *) rat)->type_id ) {
		case RuminateBackend::TypeIdStructure:
			rat->id = R_AGGREGATE_TYPE_STRUCT;
			break;
		case RuminateBackend::TypeIdUnion:
			rat->id = R_AGGREGATE_TYPE_UNION;
			break;
		case RuminateBackend::TypeIdEnum:
			rat->id = R_AGGREGATE_TYPE_ENUM;
			break;
		case RuminateBackend::TypeIdFunction:
			rat->id = R_AGGREGATE_TYPE_FUNCTION;
			return r_function_type_init((RFunctionType *) rat, error);
		default:
			g_assert_not_reached();
	}

	return true;
}

void r_aggregate_type_destroy( RAggregateType *rat ) RUMINATE_NOEXCEPT {
	switch( rat->id ) {
		case R_AGGREGATE_TYPE_STRUCT:
		case R_AGGREGATE_TYPE_UNION:
		case R_AGGREGATE_TYPE_ENUM:
			break;
		case R_AGGREGATE_TYPE_FUNCTION:
			r_function_type_destroy((RFunctionType *) rat);
			break;
		default:
			g_assert_not_reached();
	}

	if( rat->members_init ) {
		rat->members.clear();
		rat->members_init = false;
	}
}

RAggregateType *r_aggregate_type_alloc( RuminateBackend::TypeId id, GError **error ) RUMINATE_NOEXCEPT {
	switch( id ) {
		case RuminateBackend::TypeIdFunction:
			return (RAggregateType *) r_function_type_alloc(id, error);
		case RuminateBackend::TypeIdUnion:
		case RuminateBackend::TypeIdEnum:
		case RuminateBackend::TypeIdStructure: {
			RAggregateType *ret = g_slice_new(RAggregateType);
			new (ret) RAggregateType();
			return ret;
		}
		default:
			g_assert_not_reached();
	}

	g_assert_not_reached();
}

void r_aggregate_type_free( RAggregateType *rat ) RUMINATE_NOEXCEPT {
	switch( rat->id ) {
		case R_AGGREGATE_TYPE_STRUCT:
		case R_AGGREGATE_TYPE_UNION:
		case R_AGGREGATE_TYPE_ENUM:
			rat->~RAggregateType();
			g_slice_free(RAggregateType, rat);
			break;
		case R_AGGREGATE_TYPE_FUNCTION:
			r_function_type_free((RFunctionType *) rat);
			break;
		default:
			g_assert_not_reached();
	}
}

static bool init_members( RAggregateType *rat, GError **error ) RUMINATE_NOEXCEPT {
	if( !rat->members_init ) {
		RType *rt = (RType *) rat;
		if( !gxx_call(rat->members = rt->type->getMembers(IceUtil::None), error) )
			return false;
		rat->members_init = true;
	}
	return true;
}

G_BEGIN_DECLS

RAggregateTypeId r_aggregate_type_id( RAggregateType *rat, GError ** ) RUMINATE_NOEXCEPT {
	return rat->id;
}

size_t r_aggregate_type_nmembers( RAggregateType *rat, GError **error ) RUMINATE_NOEXCEPT {
	if( !init_members(rat, error) ) return 0;
	return rat->members.size();
}

RAggregateMember *r_aggregate_type_member_at( RAggregateType *rat, size_t i, GError **error ) RUMINATE_NOEXCEPT {
	if( !init_members(rat, error) ) return 0;
	// TODO: vector access could throw
	RuminateBackend::TypeMemberPrx tmp = rat->members[i];
	// TODO: Memoize RAggregateMembers
	RType *rt = (RType *) rat;
	return (RAggregateMember *) r_type_member_new(tmp, rt, error);
}

RAggregateMember *r_aggregate_type_member_by_name( RAggregateType *rat, const char *name, GError **error ) RUMINATE_NOEXCEPT {
	GError *err = NULL;
	GQuark member_name_quark, name_quark;
	size_t nmembers, i;
	RAggregateMember *member;
	RString *member_name;

	name_quark = g_quark_from_string(name);
	nmembers = r_aggregate_type_nmembers(rat, &err);
	if( err != NULL ) goto error_nmembers;
	for( i = 0; i < nmembers; i++ ) {
		member = r_aggregate_type_member_at(rat, i, error);
		if( member == NULL ) goto error_member_at;

		member_name = r_aggregate_member_name(member, error);
		if( member_name == NULL ) goto error_member_name;
		member_name_quark = r_string_quark(member_name);
		r_string_unref(member_name);

		if( member_name_quark == name_quark ) return member;

		r_type_member_unref((RTypeMember *) member);

		continue;
error_member_name:
		r_type_member_unref((RTypeMember *) member);
error_member_at:
		goto error_in_loop;
	}

error_in_loop:
error_nmembers:
	g_propagate_error(error, err);
	return NULL;
}

G_END_DECLS
