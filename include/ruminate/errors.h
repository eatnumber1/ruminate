/** Error handling facilities.
 * 
 * Every function which takes as an argument a `GError **` reports errors
 * through this pointer.
 *
 * In brief, pass `NULL` as the `GError **` argument to functions or pass a
 * pointer to a `NULL` `GError *` to recieve a `GError` when an error occurs.
 *
 * @see [GError](https://developer.gnome.org/glib/2.34/glib-Error-Reporting.html)
 * @file errors.h
 */

/** The various errors produced by ruminate.
 *
 * These will be placed in the `code` field of a `GError` produced when an error
 * occurrs.
 */
typedef enum {
	RUMINATE_ERROR_SB_INVALID,
	RUMINATE_ERROR_LLDB_ERROR,
	RUMINATE_ERROR_RANGE,
	RUMINATE_ERROR_NO_PRIMITIVE_TYPE,
	RUMINATE_ERROR_INVALID_TYPE,
	RUMINATE_ERROR_INCOMPLETE_TYPE,
	RUMINATE_ERROR_ICE,
	RUMINATE_ERROR_STDLIB,
	RUMINATE_ERROR_SHORT_READ,
	RUMINATE_ERROR_NO_PRGNAME,
	RUMINATE_ERROR_UNIMPLEMENTED,
	RUMINATE_ERROR_UNSPEC
} RuminateError;

G_BEGIN_DECLS

/// @private
GQuark ruminate_error_quark(void);

/// @private
GQuark ruminate_errno_quark(void);

G_END_DECLS

/** The error quark representing errors produced by this library.
 *
 * This quark will be placed in the `domain` field of a `GError` produced when
 * an error occurrs.
 *
 * @see [GQuark](https://developer.gnome.org/glib/2.34/glib-Quarks.html)
 */
#define RUMINATE_ERROR (ruminate_error_quark())

/** The error quark representing errors produced by the standard C library.
 *
 * This quark will be placed in the `domain` field of a `GError` produced when
 * an error occurrs.
 *
 * @see [GQuark](https://developer.gnome.org/glib/2.34/glib-Quarks.html)
 */
#define RUMINATE_ERRNO_ERROR (ruminate_errno_quark())
