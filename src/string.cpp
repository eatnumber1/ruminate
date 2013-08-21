#include <exception>
#include <sstream>

#include <stddef.h>
#include <string.h>

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/string.h"

#include "private/string.h"

RString *r_string_new( const char *init, size_t len ) RUMINATE_NOEXCEPT {
	RString *ret = g_slice_new(RString);
	new (ret) RString();
	ret->gstr = g_string_new_len(init, len);
	ret->refcnt = 1;
	return ret;
}

RString *r_string_new( const char *init ) RUMINATE_NOEXCEPT {
	return r_string_new(init, strlen(init));
}

G_BEGIN_DECLS

RString *r_string_ref( RString *rs ) RUMINATE_NOEXCEPT {
	g_atomic_int_inc(&rs->refcnt);
	return rs;
}

void r_string_unref( RString *rs ) RUMINATE_NOEXCEPT {
	if( g_atomic_int_dec_and_test(&rs->refcnt) ) {
		g_string_free(rs->gstr, true);
		rs->~RString();
		g_slice_free(RString, rs);
	}
}

const char *r_string_bytes( const RString *rs ) RUMINATE_NOEXCEPT {
	return rs->gstr->str;
}

G_END_DECLS
