#include <stddef.h>

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/refptr.h"

static void r_ptr_refcnt_free( gint *refcnt ) {
	g_slice_free(gint, refcnt);
}

void r_ptr_set_deleter( void *ptr, RPointerDeleter deleter ) {
	g_return_if_fail(ptr != NULL);
	g_dataset_set_data(ptr, "r-ptr-deleter", deleter);
}

void *r_ptr_ref( void *ptr ) {
	// TODO: Thread safety
	g_return_val_if_fail(ptr != NULL, NULL);
	gint *refcnt = g_dataset_get_data(ptr, "r-ptr-refcnt");
	if( refcnt == NULL ) {
		refcnt = g_slice_new(gint);
		g_dataset_set_data_full(ptr, "r-ptr-refcnt", refcnt, (GDestroyNotify) r_ptr_refcnt_free);
	}
	g_atomic_int_inc(refcnt);
	return ptr;
}

void r_ptr_unref( void *ptr ) {
	// TODO: Thread safety
	g_return_if_fail(ptr != NULL);
	gint *refcnt = g_dataset_get_data(ptr, "r-ptr-refcnt");
	g_assert(refcnt != NULL);
	if( g_atomic_int_dec_and_test(refcnt) ) {
		RPointerDeleter del = g_dataset_get_data(ptr, "r-ptr-deleter");
		g_dataset_destroy(ptr);
		if( del != NULL ) del(ptr);
	}
}
