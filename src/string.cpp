#include <stddef.h>
#include <string.h>

#include <glib.h>

#include "ruminate/common.h"
#include "ruminate/string.h"

#include "private/string.h"

RString *r_string_new( const char *init, size_t len ) noexcept {
	RString *ret = g_slice_new(RString);
	ret->str = g_string_new_len(init, len);
	ret->refcnt = 1;
	return ret;
}

RString *r_string_new( const char *init ) noexcept {
	return r_string_new(init, strlen(init));
}

G_BEGIN_DECLS

RString *r_string_ref( RString *rs ) noexcept {
	g_atomic_int_inc(&rs->refcnt);
	return rs;
}

void r_string_unref( RString *rs ) noexcept {
	if( g_atomic_int_dec_and_test(&rs->refcnt) ) {
		g_string_free(rs->str, true);
		g_slice_free(RString, rs);
	}
}

G_END_DECLS
