#include "config.h"

#include <exception>
#include <sstream>
#include <cstddef>

#include <Ice/Ice.h>

#include "private/glib.h"

#include "ruminate/common.h"
#include "ruminate/errors.h"

#include "private/common.h"

void unimplemented( GError **error ) RUMINATE_NOEXCEPT {
	g_set_error_literal(
		error,
		RUMINATE_ERROR,
		RUMINATE_ERROR_UNIMPLEMENTED,
		"Operation not supported"
	);
}
