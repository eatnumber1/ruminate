#include <glib.h>

#include "ruminate/errors.h"

GQuark ruminate_error_quark() {
	return g_quark_from_static_string("ruminate-error-quark");
}
