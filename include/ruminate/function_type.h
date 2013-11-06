/** An opaque struct representing a function.
 *
 * @struct RFunctionType
 * @extends RAggregateType
 */
typedef struct RFunctionType RFunctionType;

G_BEGIN_DECLS

/** Get the return type of this function.
 *
 * @memberof RFunctionType
 * @return An RType representing the return type of this function.
 */
RType * RUMINATE_EXPORT r_function_type_return_type(
	RFunctionType *type /** [in] the function to get the return type of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

G_END_DECLS
