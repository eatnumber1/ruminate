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
#include "ruminate/rumination.h"
#include "ruminate/type.h"
#include "ruminate/array_type.h"

#define _ARRAY_TYPE_CPP_

#include "private/gettid.h"
#include "private/common.h"
#include "private/type.h"
#include "private/array_type.h"

bool r_array_type_init( RArrayType *, GError ** ) RUMINATE_NOEXCEPT {
	return true;
}

void r_array_type_destroy( RArrayType * ) RUMINATE_NOEXCEPT {}

RArrayType *r_array_type_alloc( Ruminate::TypeId, GError ** ) RUMINATE_NOEXCEPT {
	RArrayType *ret = g_slice_new(RArrayType);
	new (ret) RArrayType();
	return ret;
}

void r_array_type_free( RArrayType *rat ) RUMINATE_NOEXCEPT {
	rat->~RArrayType();
	g_slice_free(RArrayType, rat);
}

G_BEGIN_DECLS

G_STATIC_ASSERT(sizeof(size_t) >= sizeof(uint64_t));

size_t r_array_type_size( RArrayType *rat, GError **error ) RUMINATE_NOEXCEPT {
	Ice::AsyncResultPtr arp;
	if( !gxx_call(arp = ((RType *) rat)->type->begin_getArraySize(gettid()), error) )
		return 0;
	rumination_hit_breakpoint();
	size_t size;
	if( !gxx_call(size = ((RType *) rat)->type->end_getArraySize(arp), error) )
		return 0;
	return size;
}

RType *r_array_type_member_type( RArrayType *rat, GError **error ) RUMINATE_NOEXCEPT {
	Ruminate::TypePrx t;
	if( !gxx_call(t = ((RType *) rat)->type->getArrayMemberType(), error) )
		return NULL;
	return r_type_new(t, ((RType *) rat)->mem, error);
}

G_END_DECLS
