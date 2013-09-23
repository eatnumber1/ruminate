#include <exception>
#include <sstream>
#include <cstddef>
#include <new>

#include <Ice/Ice.h>
#include "ice/type.h"

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/tag_type.h"
#include "ruminate/type_member.h"
#include "ruminate/record_member.h"
#include "ruminate/record_type.h"
#include "ruminate/function_type.h"

#include "private/common.h"
#include "private/memory.h"
#include "private/type_member.h"
#include "private/record_member.h"
#include "private/type.h"
#include "private/tag_type.h"
#include "private/record_type.h"
#include "private/function_type.h"

bool r_function_type_init( RFunctionType *, GError ** ) RUMINATE_NOEXCEPT {
	return true;
}

void r_function_type_destroy( RFunctionType * ) RUMINATE_NOEXCEPT {}

RFunctionType *r_function_type_alloc( Ruminate::TypeId, GError ** ) RUMINATE_NOEXCEPT {
	RFunctionType *ret = g_slice_new(RFunctionType);
	new (ret) RFunctionType();
	return ret;
}

void r_function_type_free( RFunctionType *rft ) RUMINATE_NOEXCEPT {
	rft->~RFunctionType();
	g_slice_free(RFunctionType, rft);
}

G_BEGIN_DECLS

RType *r_function_type_return_type( RFunctionType *rft, GError **error ) RUMINATE_NOEXCEPT {
	RType *rt = (RType *) rft;
	Ruminate::TypePrx t;
	if( !gxx_call(t = rt->type->getReturnType(), error) )
		return NULL;

	RMemory *rm = r_memory_new(NULL, error);
	if( rm == NULL ) return NULL;

	RType *ret = r_type_new(t, rm, NULL, error);

	r_memory_unref(rm);

	return ret;
}

G_END_DECLS
