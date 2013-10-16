/** An opaque struct representing a pointer to another type.
 *
 * @struct RPointerType
 * @extends RType
 */
typedef struct RPointerType RPointerType;

G_BEGIN_DECLS

/** Get the type that this RPointerType points to.
 *
 * @memberof RPointerType
 * @return The type that this RPointerType points to.
 */
RType * RUMINATE_EXPORT r_pointer_type_pointee(
	RPointerType *type /** [in] the type to dereference */,
	GError **error /** [in,out] see errors.h */
) RUMINATE_NOEXCEPT;

G_END_DECLS
