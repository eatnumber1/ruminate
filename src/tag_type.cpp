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
#include "ruminate/aggregate_member.h"
#include "ruminate/aggregate_type.h"
#include "ruminate/tag_type.h"

#include "private/common.h"
#include "private/memory.h"
#include "private/type.h"
#include "private/aggregate_type.h"
#include "private/tag_type.h"

bool r_tag_type_init( RTagType *rtt, GError **error ) RUMINATE_NOEXCEPT {
	switch( ((RType *) rtt)->type_id ) {
		case Ruminate::TypeIdStructure:
			rtt->id = R_TAG_TYPE_STRUCTURE;
			break;
		case Ruminate::TypeIdUnion:
			rtt->id = R_TAG_TYPE_UNION;
			break;
		case Ruminate::TypeIdEnum:
			rtt->id = R_TAG_TYPE_ENUM;
			break;
		default:
			g_assert_not_reached();
	}

	return true;
}

void r_tag_type_destroy( RTagType * ) RUMINATE_NOEXCEPT {}

RTagType *r_tag_type_alloc( Ruminate::TypeId id, GError ** ) RUMINATE_NOEXCEPT {
	switch( id ) {
		case Ruminate::TypeIdStructure:
		case Ruminate::TypeIdUnion:
		case Ruminate::TypeIdEnum: {
			RTagType *ret = g_slice_new(RTagType);
			new (ret) RTagType();
			return ret;
		}
		default:
			g_assert_not_reached();
	}

	g_assert_not_reached();
}

void r_tag_type_free( RTagType *rtt ) RUMINATE_NOEXCEPT {
	switch( rtt->id ) {
		case R_TAG_TYPE_STRUCTURE:
		case R_TAG_TYPE_UNION:
		case R_TAG_TYPE_ENUM:
			rtt->~RTagType();
			g_slice_free(RTagType, rtt);
			break;
		default:
			g_assert_not_reached();
	}
}

G_BEGIN_DECLS

RTagTypeId r_tag_type_id( RTagType *rtt, GError **error ) RUMINATE_NOEXCEPT {
	(void) error;
	return rtt->id;
}

RString *r_tag_type_name( RTagType *, GError **error ) RUMINATE_NOEXCEPT {
	unimplemented(error);
	return NULL;
}

G_END_DECLS
