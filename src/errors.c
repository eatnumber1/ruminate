#include <errno.h>

#include <glib.h>

#include "ruminate/errors.h"
#include "private/errors.h"

GQuark ruminate_errno_quark() {
	return g_quark_from_static_string("ruminate-errno-quark");
}

GQuark ruminate_error_quark() {
	return g_quark_from_static_string("ruminate-error-quark");
}

void r_perror( GError **error, const gchar *prefix ) {
	int saved_errno = errno;
	if( prefix == NULL ) {
		g_set_error(
			error,
			RUMINATE_ERRNO_ERROR,
			saved_errno,
			"%s",
			g_strerror(saved_errno)
		);
	} else {
		g_set_error(
			error,
			RUMINATE_ERRNO_ERROR,
			saved_errno,
			"%s: %s",
			prefix,
			g_strerror(saved_errno)
		);
	}
}
