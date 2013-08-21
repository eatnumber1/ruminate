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
#include "ruminate/tag_type.h"
#include "ruminate/record_member.h"
#include "ruminate/record_type.h"
#include "ruminate/function_type.h"

#define _FUNCTION_TYPE_CPP_

#include "private/common.h"
#include "private/record_member.h"
#include "private/type.h"
#include "private/tag_type.h"
#include "private/record_type.h"
#include "private/function_type.h"

bool r_function_type_init( RFunctionType *rft, GError ** ) RUMINATE_NOEXCEPT {
	new (rft) RFunctionType();
	return true;
}

void r_function_type_destroy( RFunctionType *rft ) RUMINATE_NOEXCEPT {
	rft->~RFunctionType();
}

RFunctionType *r_function_type_alloc( Ruminate::TypeId, GError ** ) RUMINATE_NOEXCEPT {
	return g_slice_new(RFunctionType);
}

void r_function_type_free( RFunctionType *rft ) RUMINATE_NOEXCEPT {
	g_slice_free(RFunctionType, rft);
}

G_BEGIN_DECLS

RType *r_function_type_return_type( RFunctionType *rft, GError **error ) RUMINATE_NOEXCEPT {
	Ruminate::TypePrx t;
	if( !gxx_call(t = ((RType *) rft)->type->getReturnType(), error) )
		return NULL;

	return r_type_new(t, error);
}

G_END_DECLS
