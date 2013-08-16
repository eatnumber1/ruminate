#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <cstddef>

#include <glib.h>

#include <Ice/Ice.h>

#include "ruminate/common.h"
#include "ruminate/errors.h"

#define _COMMON_CPP_

#include "private/common.h"

template gxx_call_proto(std::string);
template gxx_call_proto(bool);
template gxx_call_proto(size_t);

void unimplemented( GError **error ) noexcept {
	g_set_error_literal(
		error,
		RUMINATE_ERROR,
		RUMINATE_ERROR_UNIMPLEMENTED,
		"Operation not supported"
	);
}

bool gxx_call( const std::function<void()> &func, GError **error ) noexcept {
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
