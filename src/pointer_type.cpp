#include "config.h"

#include <exception>
#include <sstream>
#include <cstddef>
#include <new>

#include "private/ice.h"
#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/pointer_type.h"

#include "private/common.h"
#include "private/type.h"
#include "private/pointer_type.h"

bool r_pointer_type_init( RPointerType *, GError ** ) RUMINATE_NOEXCEPT {
	return true;
}

void r_pointer_type_destroy( RPointerType * ) RUMINATE_NOEXCEPT {}

RPointerType *r_pointer_type_alloc( RuminateBackend::TypeId, GError ** ) RUMINATE_NOEXCEPT {
	RPointerType *ret = g_slice_new(RPointerType);
	new (ret) RPointerType();
	return ret;
}

void r_pointer_type_free( RPointerType *rpt ) RUMINATE_NOEXCEPT {
	rpt->~RPointerType();
	g_slice_free(RPointerType, rpt);
}

G_BEGIN_DECLS

RType *r_pointer_type_pointee( RPointerType *rpt, GError **error ) RUMINATE_NOEXCEPT {
	RuminateBackend::TypePrx t;
	if( !gxx_call(t = ((RType *) rpt)->type->getPointeeType(), error) )
		return NULL;

	return r_type_new(t, error);
}

G_END_DECLS
