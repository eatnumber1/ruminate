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

#define _TAG_TYPE_CPP_

#include "private/common.h"
#include "private/type.h"
#include "private/tag_type.h"
#include "private/record_type.h"

bool r_tag_type_init( RTagType *rtt, GError **error ) noexcept {
	switch( ((RType *) rtt)->type_id ) {
		case Ruminate::TypeIdStructure:
			rtt->id = R_TAG_TYPE_RECORD;
			return r_record_type_init((RRecordType *) rtt, error);
		default:
			g_assert_not_reached();
	}

	g_assert_not_reached();
}

void r_tag_type_destroy( RTagType *rtt ) noexcept {
	switch( rtt->id ) {
		case R_TAG_TYPE_RECORD:
			r_record_type_destroy((RRecordType *) rtt);
			break;
		default:
			g_assert_not_reached();
	}
}

RTagType *r_tag_type_alloc( Ruminate::TypeId id, GError **error ) noexcept {
	switch( id ) {
		case Ruminate::TypeIdStructure:
			return (RTagType *) r_record_type_alloc(id, error);
		default:
			g_assert_not_reached();
	}

	g_assert_not_reached();
}

void r_tag_type_free( RTagType *rtt ) noexcept {
	switch( rtt->id ) {
		case R_TAG_TYPE_RECORD:
			r_record_type_free((RRecordType *) rtt);
			break;
		default:
			g_assert_not_reached();
	}
}

G_BEGIN_DECLS

RTagTypeId r_tag_type_id( RTagType *rtt, GError **error ) noexcept {
	(void) error;
	return rtt->id;
}

const RString *r_tag_type_name( RTagType *, GError **error ) noexcept {
	unimplemented(error);
	return NULL;
}

G_END_DECLS