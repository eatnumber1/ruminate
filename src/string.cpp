#include "config.h"

#include <exception>
#include <sstream>
#include <new>

#include <stddef.h>
#include <string.h>

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/string.h"

#include "private/string.h"

RString *r_string_new_cxx( const std::string &init ) RUMINATE_NOEXCEPT {
	return r_string_new_len(init.c_str(), init.size());
}

G_BEGIN_DECLS

RString *r_string_new_len( const char *init, size_t len ) RUMINATE_NOEXCEPT {
	RString *ret = g_slice_new(RString);
	new (ret) RString();
	ret->gstr = g_string_new_len(init, (gssize) len);
	ret->refcnt = 1;
	ret->quark.initialized = false;
	return ret;
}

RString *r_string_new( const char *init ) RUMINATE_NOEXCEPT {
	return r_string_new_len(init, strlen(init));
}

RString *r_string_ref( RString *rs ) RUMINATE_NOEXCEPT {
	g_atomic_int_inc(&rs->refcnt);
	return rs;
}

void r_string_unref( RString *rs ) RUMINATE_NOEXCEPT {
	if( g_atomic_int_dec_and_test(&rs->refcnt) ) {
		g_string_free(rs->gstr, true);
		rs->~RString();
		g_slice_free(RString, rs);
		// TODO: free ret->quark.value?
	}
}

const char *r_string_bytes( RString *rs ) RUMINATE_NOEXCEPT {
	return rs->gstr->str;
}

size_t r_string_length( RString *rs ) RUMINATE_NOEXCEPT {
	return rs->gstr->len;
}

GQuark r_string_quark( RString *rs ) RUMINATE_NOEXCEPT {
	if( rs->quark.initialized ) return rs->quark.value;

	rs->quark.value = g_quark_from_string(rs->gstr->str);
	rs->quark.initialized = true;
	return rs->quark.value;
}

G_END_DECLS
