#include <exception>
#include <sstream>
#include <cstddef>

#include <stdint.h>

#include <Ice/Ice.h>
#include "ice/type.h"

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/type_member.h"
#include "ruminate/array_type.h"
#include "ruminate/frame.h"
#include "ruminate/rumination.h"

#include "private/gettid.h"
#include "private/common.h"
#include "private/memory.h"
#include "private/type.h"
#include "private/type_member.h"
#include "private/array_type.h"

bool r_array_type_init( RArrayType *rat, GError ** ) RUMINATE_NOEXCEPT {
	rat->members.valid = false;
	return true;
}

void r_array_type_destroy( RArrayType *rat ) RUMINATE_NOEXCEPT {
	if( rat->members.valid ) {
		rat->members.value.clear();
		rat->members.valid = false;
	}
}

RArrayType *r_array_type_alloc( Ruminate::TypeId, GError ** ) RUMINATE_NOEXCEPT {
	RArrayType *ret = g_slice_new(RArrayType);
	new (ret) RArrayType();
	return ret;
}

void r_array_type_free( RArrayType *rat ) RUMINATE_NOEXCEPT {
	rat->~RArrayType();
	g_slice_free(RArrayType, rat);
}

// TODO: Dedup this with RRecordType?
static bool init_members( RArrayType *rat, GError **error ) RUMINATE_NOEXCEPT {
	if( !rat->members.valid ) {
		if( !gxx_call(rat->members.value = ((RType *) rat)->type->getMembers(), error) )
			return false;
		rat->members.valid = true;
	}
	return true;
}

G_BEGIN_DECLS

G_STATIC_ASSERT(sizeof(size_t) >= sizeof(uint64_t));

size_t r_array_type_size( RArrayType *rat, GError **error ) RUMINATE_NOEXCEPT {
	if( !init_members(rat, error) ) return 0;
	// TODO: size() can throw
	return rat->members.value.size();
}

RTypeMember *r_array_type_member_at( RArrayType *rat, size_t idx, GError **error ) RUMINATE_NOEXCEPT {
	if( !init_members(rat, error) ) return NULL;
	// TODO: vector access could throw
	Ruminate::TypeMemberPrx tmp = rat->members.value[idx];
	off_t offset;
	if( !_r_type_member_offset(tmp, &offset, error) ) return NULL;
	// TODO: Memoize RTypeMembers
	RType *rt = (RType *) rat;
	return r_type_member_new(tmp, R_TYPE_MEMBER_ARRAY, rt->ptr, ((uint8_t *) rt->cur) + offset, error);
}

G_END_DECLS
