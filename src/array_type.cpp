#include "config.h"

#include <exception>
#include <sstream>
#include <cstddef>

#include <stdint.h>

#include <Ice/Ice.h>
#include "ice/debugger_factory.h"
#include "ice/debugger.h"
#include "ice/type.h"

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/type_member.h"
#include "ruminate/array_type.h"
#include "ruminate/frame.h"
#include "ruminate/ruminate.h"

#include "private/gettid.h"
#include "private/common.h"
#include "private/type.h"
#include "private/type_member.h"
#include "private/array_type.h"
#include "private/ruminate.h"

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

RArrayType *r_array_type_alloc( RuminateBackend::TypeId, GError ** ) RUMINATE_NOEXCEPT {
	RArrayType *ret = g_slice_new(RArrayType);
	new (ret) RArrayType();
	return ret;
}

void r_array_type_free( RArrayType *rat ) RUMINATE_NOEXCEPT {
	rat->~RArrayType();
	g_slice_free(RArrayType, rat);
}

// TODO: Dedup this with RAggregateType?
static bool init_members( RArrayType *rat, GError **error ) RUMINATE_NOEXCEPT {
	if( !rat->members.valid ) {
		RType *rt = (RType *) rat;
		// TODO: Handle exceptions
		Ice::AsyncResultPtr arp;
		if( !gxx_call(arp = rt->type->begin_getMembers(gettid()), error) )
			return false;
		ruminate_hit_breakpoint();
		if( !gxx_call(rat->members.value = rt->type->end_getMembers(arp), error) )
			return false;
		rat->members.valid = true;
	}
	return true;
}

G_BEGIN_DECLS

R_STATIC_ASSERT(sizeof(size_t) >= sizeof(uint64_t));

size_t r_array_type_size( RArrayType *rat, GError **error ) RUMINATE_NOEXCEPT {
	if( !init_members(rat, error) ) return 0;
	// TODO: size() can throw
	return rat->members.value.size();
}

RTypeMember *r_array_type_member_at( RArrayType *rat, size_t idx, GError **error ) RUMINATE_NOEXCEPT {
	if( !init_members(rat, error) ) return NULL;
	// TODO: vector access could throw
	RuminateBackend::TypeMemberPrx tmp = rat->members.value[idx];
	// TODO: Memoize RTypeMembers
	return r_type_member_new(tmp, (RType *) rat, error);
}

G_END_DECLS
