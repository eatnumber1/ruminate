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

/** Get the name of this function.
 *
 * @memberof RFunctionType
 * @return A RString containing the name of this function.
 */
RString * RUMINATE_EXPORT r_function_type_name(
	RFunctionType *type /** [in] the function to get the name of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

G_END_DECLS
