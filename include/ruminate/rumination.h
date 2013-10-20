/** Top-level and utility functions.
 *
 * @file rumination.h
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
bool RUMINATE_EXPORT rumination_destroy(
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Initialize the ruminate framework.
 *
 * This must be called before any other ruminate functions.
 *
 * @return Whether or not an error occurred.
 */
bool RUMINATE_EXPORT rumination_init(
	int *arglen /** [in,out] a pointer to the number of arguments */,
	char *args[] /** [in,out] an array of string arguments */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/// @private
bool RUMINATE_EXPORT rumination_begin_get_type_by_variable_name( const char *, GError ** ) RUMINATE_NOEXCEPT;
/// @private
RType * RUMINATE_EXPORT rumination_end_get_type_by_variable_name( void *, GError ** ) RUMINATE_NOEXCEPT;

/** Generate a backtrace.
 *
 * This function generates a backtrace of the caller's call stack.
 *
 * @return A pointer to an RFrameList representing the frames found in the
 *         caller's call stack. This RFrameList must be freed using
 *         r_frame_list_unref().
 */
RFrameList * RUMINATE_EXPORT rumination_backtrace(
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/// @private
__attribute__((noinline))
void RUMINATE_EXPORT rumination_hit_breakpoint() RUMINATE_NOEXCEPT;

G_END_DECLS

/** Get the type of an expression.
 *
 * Gets an instance of an RType representing the type of the provided
 * expression.
 *
 * Note that you must first have initialized the ruminate library via a call to
 * rumination_init().
 *
 * @param[in] expr The expression to determine the type of.
 * @param[out] error see errors.h
 * @return A pointer to an RType or `NULL` if an error occurred. This RType
 *         must be freed using r_type_unref().
 */
#define rumination_get_type(expr, error) ({ \
	__typeof__(expr) *_expr = g_malloc(sizeof(__typeof__(expr))); \
	*_expr = (expr); \
	RType *ret; \
	if( !rumination_begin_get_type_by_variable_name("_expr", error) ) { \
		ret = NULL; \
	} else { \
		rumination_hit_breakpoint(); \
		ret = rumination_end_get_type_by_variable_name(_expr, error); \
	} \
	ret; \
})
