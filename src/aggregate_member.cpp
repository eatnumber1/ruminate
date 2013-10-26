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
#include "private/string.h"
#include "private/memory.h"
#include "private/type.h"
#include "private/type_member.h"
#include "private/aggregate_member.h"

bool r_aggregate_member_init( RAggregateMember *rm, GError **error ) RUMINATE_NOEXCEPT {
	bool bitfield;

	if( !gxx_call(bitfield = ((RTypeMember *) rm)->member->isBitfield(), error) )
		return false;

	rm->id = bitfield ? R_AGGREGATE_MEMBER_BITFIELD : R_AGGREGATE_MEMBER_OTHER;
	rm->name = NULL;

	return true;
}

void r_aggregate_member_destroy( RAggregateMember *rm ) RUMINATE_NOEXCEPT {
	if( rm->name != NULL ) r_string_unref(rm->name);
}

RAggregateMember *r_aggregate_member_alloc( GError ** ) RUMINATE_NOEXCEPT {
	RAggregateMember *ret = g_slice_new(RAggregateMember);
	new (ret) RAggregateMember();
	return ret;
}

void r_aggregate_member_free( RAggregateMember *rm ) RUMINATE_NOEXCEPT {
	rm->~RAggregateMember();
	g_slice_free(RAggregateMember, rm);
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
