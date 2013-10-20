/** An opaque struct representing a typedef'ed type.
 *
 * @struct RTypedefType
 * @extends RType
 */
typedef struct RTypedefType RTypedefType;

G_BEGIN_DECLS

/** Get the canonical type of this type
 *
 * A canonical type strips away all typedefs contained within this type.
 *
 * For example with the following code,
 * @code{.c}
 * typedef char *String;
 * typedef String *StringArray;
 * @endcode
 * calling r_typedef_type_canonical() on a RTypedefType representing a
 * `StringArray` will return an RType representing `char **`.
 *
 * @todo Be able to single step through non-canonical types of an RTypedefType.
 * @return An RType representing the canonical type of this RTypedefType.
 * @memberof RTypedefType
 */
RType * RUMINATE_EXPORT r_typedef_type_canonical(
	RTypedefType * /** [in] the type to canonicalize */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

G_END_DECLS
