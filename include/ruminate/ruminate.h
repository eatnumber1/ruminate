/** Top-level and utility functions.
 *
 * @file include/ruminate/ruminate.h
 */

G_BEGIN_DECLS

/** De-initialize the ruminate framework.
 *
 * This function frees all internal resources of the ruminate framework.
 * Undefined behavior results if any ruminate framework functions are called
 * after this function returns `true`.
 *
 * @return Whether or not an error occurred.
 */
bool RUMINATE_EXPORT ruminate_destroy(
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Initialize the ruminate framework.
 *
 * This must be called before any other ruminate functions.
 *
 * The argument \a program_name must either be the name of this program or `NULL`.
 * If null, `g_get_prgname()` will be called to get the name of this program. If
 * the program name has not been previously set via a call to `g_set_prgname()`,
 * an error will occur.
 *
 * @return Whether or not an error occurred.
 * @see [g_set_prgname](https://developer.gnome.org/glib/2.28/glib-Miscellaneous-Utility-Functions.html#g-set-prgname)
 */
bool RUMINATE_EXPORT ruminate_init(
	const char *program_name /** [in] the name of this program, e.g. `argv[0]` or `NULL` */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Generate a backtrace.
 *
 * This function generates a backtrace of the caller's call stack.
 *
 * @return A pointer to an RFrameList representing the frames found in the
 *         caller's call stack. This RFrameList must be freed using
 *         r_frame_list_unref().
 * @todo This method should return a GPtrArray rather than a custom list
 *       implementation.
 */
RFrameList * RUMINATE_EXPORT ruminate_backtrace(
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Retrieve RTypes by name
*
* This function retrieves all the types which are named \a type_name.
* 
* @return A GPtrArray of the types which are named \a type_name
*/
GPtrArray * RUMINATE_EXPORT ruminate_get_types_by_name(
	const char *type_name /** [in] the name of the types to find */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/// @todo document
RType * RUMINATE_EXPORT ruminate_get_type_by_variable_name(
	const char *,
	GError **
) RUMINATE_NOEXCEPT;

/** Get the name of a function by address.
 *
 * @return A RString containing the name of the function.
 */
RString * RUMINATE_EXPORT ruminate_get_function_name(
	void *addr /** [in] the address of the function to get the name of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

G_END_DECLS

/** Get the type of an expression.
 *
 * Gets an instance of an RType representing the type of the provided
 * expression.
 *
 * Note that you must first have initialized the ruminate library via a call to
 * ruminate_init().
 *
 * @param[in] expr The expression to determine the type of.
 * @param[out] error see errors.h
 * @return A pointer to an RType or `NULL` if an error occurred. This RType
 *         must be freed using r_type_unref().
 */
#define ruminate_get_type(expr, error) \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wgnu-statement-expression\"") \
	({ \
		__typeof__(expr) _expr = (expr); \
		(void) _expr; \
		ruminate_get_type_by_variable_name("_expr", error); \
	}) \
	_Pragma("clang diagnostic pop")
