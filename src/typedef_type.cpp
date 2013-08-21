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
#include "ruminate/typedef_type.h"

#define _TYPEDEF_TYPE_CPP_

#include "private/common.h"
#include "private/type.h"
#include "private/typedef_type.h"

bool r_typedef_type_init( RTypedefType *rtt, GError ** ) RUMINATE_NOEXCEPT {
	new (rtt) RTypedefType();
	return true;
}

void r_typedef_type_destroy( RTypedefType *rtt ) RUMINATE_NOEXCEPT {
	rtt->~RTypedefType();
}

RTypedefType *r_typedef_type_alloc( Ruminate::TypeId, GError ** ) RUMINATE_NOEXCEPT {
	return g_slice_new(RTypedefType);
}

void r_typedef_type_free( RTypedefType *rtt ) RUMINATE_NOEXCEPT {
	g_slice_free(RTypedefType, rtt);
}

G_BEGIN_DECLS

RType *r_typedef_type_canonical( RTypedefType *rtt, GError **error ) RUMINATE_NOEXCEPT {
	Ruminate::TypePrx t;
	if( !gxx_call(t = ((RType *) rtt)->type->getCanonicalType(), error) )
		return NULL;

	return r_type_new(t, error);
}

G_END_DECLS
