#ifndef _RUMINATE_ERRORS_H_
#define _RUMINATE_ERRORS_H_

#include <glib.h>

G_BEGIN_DECLS

GQuark ruminate_error_quark();

#define RUMINATE_ERROR ruminate_error_quark()

typedef enum {
	RUMINATE_ERROR_STD_EXCEPTION,
	RUMINATE_ERROR_SB_INVALID,
	RUMINATE_ERROR_LLDB_ERROR,
	RUMINATE_ERROR_RANGE,
	RUMINATE_ERROR_ERRNO
} RuminateError;

G_END_DECLS

#endif
