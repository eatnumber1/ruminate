#include <exception>
#include <sstream>
#include <cstddef>

#include <Ice/Ice.h>

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"

#define _COMMON_CPP_

#include "private/common.h"

#if 0
template gxx_call_proto(std::string);
template gxx_call_proto(bool);
template gxx_call_proto(size_t);
#endif

void unimplemented( GError **error ) RUMINATE_NOEXCEPT {
	g_set_error_literal(
		error,
		RUMINATE_ERROR,
		RUMINATE_ERROR_UNIMPLEMENTED,
		"Operation not supported"
	);
}

#if 0
bool gxx_call( const std::function<void()> &func, GError **error ) RUMINATE_NOEXCEPT {
	try {
		func();
		return true;
	} catch( Ice::Exception &ex ) {
		// If we throw here, meh.
		std::ostringstream ss;
		ss << ex;
		g_set_error(
			error,
			RUMINATE_ERROR,
			RUMINATE_ERROR_ICE,
			"%s",
			ss.str().c_str()
		);
	} catch( std::exception &ex ) {
		g_set_error(
			error,
			RUMINATE_ERROR,
			RUMINATE_ERROR_STDLIB,
			"%s",
			ex.what()
		);
	} catch(...) {
		g_set_error_literal(
			error,
			RUMINATE_ERROR,
			RUMINATE_ERROR_UNSPEC,
			"Unspecified error"
		);
	}

	return false;
}
#endif
