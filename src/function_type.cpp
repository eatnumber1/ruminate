#include <exception>
#include <sstream>
#include <cstddef>
#include <new>

#include <stdint.h>

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
#include "ruminate/type_member.h"
#include "ruminate/function_type.h"
#include "ruminate/frame.h"
#include "ruminate/ruminate.h"

#include "private/common.h"
#include "private/gettid.h"
#include "private/memory.h"
#include "private/string.h"
#include "private/type_member.h"
#include "private/aggregate_member.h"
#include "private/type.h"
#include "private/aggregate_type.h"
#include "private/tag_type.h"
#include "private/function_type.h"

bool r_function_type_init( RFunctionType *rft, GError ** ) RUMINATE_NOEXCEPT {
	rft->name = NULL;
	return true;
}

void r_function_type_destroy( RFunctionType *rft ) RUMINATE_NOEXCEPT {
	if( rft->name != NULL ) r_string_unref(rft->name);
}

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

RString *r_function_type_name( RFunctionType *rft, GError **error ) RUMINATE_NOEXCEPT {
	if( rft->name == NULL ) {
		std::string name;
		RType *rt = (RType *) rft;
		Ice::AsyncResultPtr arp;
		if( !gxx_call(arp = rt->type->begin_getName(gettid()), error) )
			return NULL;
		ruminate_hit_breakpoint();
		if( !gxx_call(name = rt->type->end_getName(arp), error) )
			return NULL;
		rft->name = r_string_new(name.c_str());
	}
	return r_string_ref(rft->name);
}

G_END_DECLS
