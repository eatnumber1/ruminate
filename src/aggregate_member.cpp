#include <exception>
#include <sstream>
#include <cstddef>
#include <new>

#include <stdint.h>

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
#include "ruminate/enum_member.h"

#include "private/common.h"
#include "private/string.h"
#include "private/memory.h"
#include "private/type.h"
#include "private/type_member.h"
#include "private/aggregate_member.h"
#include "private/enum_member.h"

bool r_aggregate_member_init( RAggregateMember *rm, RType *container, GError **error ) RUMINATE_NOEXCEPT {
	switch( container->type_id ) {
		case RuminateBackend::TypeIdStructure:
		case RuminateBackend::TypeIdUnion: {
			bool bitfield;

			if( !gxx_call(bitfield = ((RTypeMember *) rm)->member->isBitfield(), error) )
				return false;

			rm->id = bitfield ? R_AGGREGATE_MEMBER_BITFIELD : R_AGGREGATE_MEMBER_OTHER;
			break;
		}
		case RuminateBackend::TypeIdEnum:
			rm->id = R_AGGREGATE_MEMBER_ENUM;
			break;
		case RuminateBackend::TypeIdFunction:
			rm->id = R_AGGREGATE_MEMBER_OTHER;
			break;
		default:
			g_assert_not_reached();
	}

	switch( rm->id ) {
		case R_AGGREGATE_MEMBER_ENUM:
			if( !r_enum_member_init((REnumMember *) rm, container, error) )
				return false;
			break;
		default:
			break;
	}

	return true;
}

void r_aggregate_member_destroy( RAggregateMember *rm ) RUMINATE_NOEXCEPT {
	switch( rm->id ) {
		case R_AGGREGATE_MEMBER_ENUM:
			r_enum_member_destroy((REnumMember *) rm);
			break;
		default:
			break;
	}

	if( rm->name != NULL ) r_string_unref(rm->name);
}

RAggregateMember *r_aggregate_member_alloc( RType *container, GError **error ) RUMINATE_NOEXCEPT {
	RAggregateMember *ret;
	switch( container->type_id ) {
		case RuminateBackend::TypeIdEnum:
			ret = (RAggregateMember *) r_enum_member_alloc(container, error);
			break;
		case RuminateBackend::TypeIdStructure:
		case RuminateBackend::TypeIdUnion:
		case RuminateBackend::TypeIdFunction:
			ret = g_slice_new(RAggregateMember);
			new (ret) RAggregateMember();
			break;
		default:
			g_assert_not_reached();
	}
	return ret;
}

void r_aggregate_member_free( RAggregateMember *rm ) RUMINATE_NOEXCEPT {
	switch( rm->id ) {
		case R_AGGREGATE_MEMBER_ENUM:
			r_enum_member_free((REnumMember *) rm);
			break;
		default:
			rm->~RAggregateMember();
			g_slice_free(RAggregateMember, rm);
	}
}

G_BEGIN_DECLS

RAggregateMemberId r_aggregate_member_id( RAggregateMember *rm, GError ** ) RUMINATE_NOEXCEPT {
	return rm->id;
}

RString *r_aggregate_member_name( RAggregateMember *rm, GError **error ) RUMINATE_NOEXCEPT {
	if( rm->name == NULL ) {
		std::string name;
		if( !gxx_call(name = ((RTypeMember *) rm)->member->getName(), error) )
			return NULL;

		rm->name = r_string_new(name.c_str());
	}

	return r_string_ref(rm->name);
}

G_END_DECLS
