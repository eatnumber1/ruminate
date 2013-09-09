#include <new>

#include "private/glib.h"

#include "ruminate/common.h"

#include "private/common.h"
#include "private/memory.h"

RMemory *r_memory_new( void *ptr, GError ** ) RUMINATE_NOEXCEPT {
	g_return_val_if_fail(ptr != NULL, NULL);
	RMemory *ret = g_slice_new(RMemory);
	new (ret) RMemory();
	ret->next = NULL;
	ret->ptr = ptr;
	ret->refcnt = 1;
	return ret;
}

RMemory *r_memory_new( void *ptr, RMemory *rm, GError **error ) RUMINATE_NOEXCEPT {
	g_assert(ptr != NULL);
	g_assert(rm != NULL);
	RMemory *ret = r_memory_new(ptr, error);
	if( ret == NULL ) return NULL;
	ret->next = r_memory_ref(rm);
	return ret;
}

RMemory *r_memory_ref( RMemory *rm ) RUMINATE_NOEXCEPT {
	g_return_val_if_fail(rm != NULL, NULL);
	g_atomic_int_inc(&rm->refcnt);
	return rm;
}

void r_memory_unref( RMemory *rm ) RUMINATE_NOEXCEPT {
	g_return_if_fail(rm != NULL);
	if( g_atomic_int_dec_and_test(&rm->refcnt) ) {
		if( rm->next != NULL ) r_memory_unref(rm->next);
		g_free(rm->ptr);
		g_slice_free(RMemory, rm);
	}
}
