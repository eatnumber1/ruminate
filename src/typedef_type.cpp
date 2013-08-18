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
#include "ruminate/type.h"
#include "ruminate/typedef_type.h"

#define _TYPEDEF_TYPE_CPP_

#include "private/common.h"
#include "private/type.h"
#include "private/typedef_type.h"

bool r_typedef_type_init( RTypedefType *, GError ** ) noexcept {
	return true;
}

void r_typedef_type_destroy( RTypedefType * ) noexcept {}

RTypedefType *r_typedef_type_alloc( Ruminate::TypeId, GError ** ) noexcept {
	return g_slice_new(RTypedefType);
}

void r_typedef_type_free( RTypedefType *rtt ) noexcept {
	g_slice_free(RTypedefType, rtt);
}

G_BEGIN_DECLS

RType *r_typedef_type_canonical( RTypedefType *rtt, GError **error ) noexcept {
	Ruminate::TypePrx t;
	if( !gxx_call([rtt, &t](){ t = ((RType *) rtt)->type->getCanonicalType(); }, error) )
		return NULL;

	return r_type_new(t, error);
}

G_END_DECLS
