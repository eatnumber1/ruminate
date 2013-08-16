#ifndef _RUMINATE_ERRORS_H_
#define _RUMINATE_ERRORS_H_

G_BEGIN_DECLS

GQuark ruminate_error_quark();

#define RUMINATE_ERROR ruminate_error_quark()

typedef enum {
	RUMINATE_ERROR_SB_INVALID,
	RUMINATE_ERROR_LLDB_ERROR,
	RUMINATE_ERROR_RANGE,
	RUMINATE_ERROR_NO_PRIMITIVE_TYPE,
	RUMINATE_ERROR_INVALID_TYPE,
	RUMINATE_ERROR_INCOMPLETE_TYPE,
	RUMINATE_ERROR_ICE,
	RUMINATE_ERROR_STDLIB,
	RUMINATE_ERROR_ERRNO,
	RUMINATE_ERROR_UNIMPLEMENTED,
	RUMINATE_ERROR_UNSPEC
} RuminateError;

G_END_DECLS

#endif
