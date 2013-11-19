#include "config.h"

#include <cstring>

#include <stddef.h>

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"
#include "ruminate/string.h"
#include "ruminate/type.h"
#include "ruminate/pointer_type.h"

#include "ruminate/memory.h"

#include "private/memory.h"

RMemory *r_memory_new( RType *rt, size_t size ) RUMINATE_NOEXCEPT {
	// TODO: Check for overflow
	RMemory *rm = (RMemory *) g_malloc(sizeof(RMemory) + size);

	rm->refcnt = 1;
	rm->size = size;
	rm->type = r_type_ref(rt);

	return rm;
}

RMemory * RUMINATE_NONNULL(1) r_memory_realloc( RMemory * rm, size_t size ) RUMINATE_NOEXCEPT {
	if( size == 0 ) {
		if( rm != NULL ) r_memory_unref(rm);
		return NULL;
	}

	// TODO: Check for overflow.
	return (RMemory *) g_realloc(rm, size + sizeof(RMemory));
}

void r_memory_unref( RMemory *rm ) RUMINATE_NOEXCEPT {
	if( g_atomic_int_dec_and_test(&rm->refcnt) ) {
		r_type_unref(rm->type);
		g_free(rm);
	}
}

RMemory *r_memory_ref( RMemory *rm ) RUMINATE_NOEXCEPT {
	g_atomic_int_inc(&rm->refcnt);
	return rm;
}

RMemory *r_memory_from_opaque( void *mem ) RUMINATE_NOEXCEPT {
	return (RMemory *) (((char *) mem) - offsetof(RMemory, data));
}

void *r_memory_to_opaque( RMemory *mem ) RUMINATE_NOEXCEPT {
	return ((char *) mem) + offsetof(RMemory, data);
}

G_BEGIN_DECLS

void *r_mem_malloc_fn( RType *rt, GError **error ) RUMINATE_NOEXCEPT {
	GError *err = NULL;

	size_t size = r_type_size(rt, &err);
	if( err != NULL ) goto error_type_size;

	return r_mem_malloc_sized_fn(rt, size, error);
error_type_size:
	g_propagate_error(error, err);
	return NULL;
}

void *_r_mem_malloc_sized( RType *ptrtype, size_t size, GError **error ) RUMINATE_NOEXCEPT {
	RType *type = NULL;
	void *ret = NULL;
	
	type = r_pointer_type_pointee((RPointerType *) ptrtype, error);
	if( type == NULL ) goto error_pt_pointee;

	ret = r_mem_malloc_sized_fn(type, size, error);
	if( error == NULL ) goto error_malloc_sized_fn;

	r_type_unref(ptrtype);
	return ret;
error_malloc_sized_fn:
	r_type_unref(type);
error_pt_pointee:
	r_type_unref(ptrtype);
	return NULL;
}

void *r_mem_malloc_sized_fn( RType *rt, size_t size, GError **error ) RUMINATE_NOEXCEPT {
	GError *err = NULL;

	size_t type_size = r_type_size(rt, &err);
	if( err != NULL ) goto error_typ_size;

	// TODO: Make this set GError rather than assert
	g_assert(type_size <= size);

	return r_memory_to_opaque(r_memory_new(rt, size));
error_typ_size:
	g_propagate_error(error, err);
	return NULL;
}

void *r_mem_calloc_fn( RType *rt, size_t nmemb, GError **error ) RUMINATE_NOEXCEPT {
	GError *err = NULL;

	size_t size = r_type_size(rt, &err);
	if( err != NULL ) goto error_type_size;

	return r_mem_calloc_sized_fn(rt, size, nmemb, error);

error_type_size:
	g_propagate_error(error, err);
	return NULL;
}

void *_r_mem_calloc_sized( RType *ptrtype, size_t size, size_t nmemb, GError **error ) RUMINATE_NOEXCEPT {
	RType *type = NULL;
	void *ret = NULL;
	
	type = r_pointer_type_pointee((RPointerType *) ptrtype, error);
	if( type == NULL ) goto error_pt_pointee;

	ret = r_mem_calloc_sized_fn(type, size, nmemb, error);
	if( error == NULL ) goto error_calloc_sized_fn;

	r_type_unref(ptrtype);
	return ret;
error_calloc_sized_fn:
	r_type_unref(type);
error_pt_pointee:
	r_type_unref(ptrtype);
	return NULL;
}

void *r_mem_calloc_sized_fn( RType *rt, size_t size, size_t nmemb, GError **error ) RUMINATE_NOEXCEPT {
	// TODO: Handle overflow
	size_t real_size = size * nmemb;
	void *ret = r_mem_malloc_sized_fn(rt, real_size, error);
	if( ret == NULL ) return NULL;
	memset(ret, 0, real_size);
	return ret;
}

void *r_mem_realloc( void *mem, size_t size ) RUMINATE_NOEXCEPT {
	if( mem == NULL ) return NULL;
	return r_memory_realloc(r_memory_from_opaque(mem), size);
}

size_t r_mem_size( void *mem ) RUMINATE_NOEXCEPT {
	return r_memory_from_opaque(mem)->size;
}

void *r_mem_ref( void *mem ) RUMINATE_NOEXCEPT {
	r_memory_ref(r_memory_from_opaque(mem));
	return mem;
}

void r_mem_unref( void *mem ) RUMINATE_NOEXCEPT {
	r_memory_unref(r_memory_from_opaque(mem));
}

RType *r_mem_type( void *mem ) RUMINATE_NOEXCEPT {
	return r_type_ref(r_memory_from_opaque(mem)->type);
}

G_END_DECLS
