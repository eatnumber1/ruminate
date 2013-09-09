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

#define _RECORD_MEMBER_CPP_

#include "private/common.h"
#include "private/string.h"
#include "private/value.h"
#include "private/type.h"
#include "private/type_member.h"
#include "private/record_member.h"

bool r_record_member_init( RRecordMember *rm, GError **error ) RUMINATE_NOEXCEPT {
	bool bitfield;

	if( !gxx_call(bitfield = ((RTypeMember *) rm)->member->isBitfield(), error) )
		return false;

	rm->id = bitfield ? R_RECORD_MEMBER_BITFIELD : R_RECORD_MEMBER_OTHER;
	rm->name = NULL;

	return true;
}

void r_record_member_destroy( RRecordMember *rm ) RUMINATE_NOEXCEPT {
	if( rm->name != NULL ) r_string_unref(rm->name);
}

RRecordMember *r_record_member_alloc( GError ** ) RUMINATE_NOEXCEPT {
	RRecordMember *ret = g_slice_new(RRecordMember);
	new (ret) RRecordMember();
	return ret;
}

void r_record_member_free( RRecordMember *rm ) RUMINATE_NOEXCEPT {
	rm->~RRecordMember();
	g_slice_free(RRecordMember, rm);
}

G_BEGIN_DECLS

RRecordMemberId r_record_member_id( RRecordMember *rm, GError ** ) RUMINATE_NOEXCEPT {
	return rm->id;
}

RString *r_record_member_name( RRecordMember *rm, GError **error ) RUMINATE_NOEXCEPT {
	if( rm->name == NULL ) {
		std::string name;
		if( !gxx_call(name = ((RTypeMember *) rm)->member->getName(), error) )
			return NULL;

		rm->name = r_string_new(name.c_str());
	}

	return r_string_ref(rm->name);
}

G_END_DECLS
