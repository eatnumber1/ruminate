/** An opaque struct representing an array type.
 *
 * @struct RArrayType
 * @extends RType
 */
typedef struct RArrayType RArrayType;

G_BEGIN_DECLS

/** Get the size of this array.
 *
 * @return the size of the array
 * @memberof RArrayType
 */
size_t RUMINATE_EXPORT r_array_type_size(
	RArrayType *type /** [in] the type to get the size of */,
	GError **error /** [in,out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get the type of a member of this array.
 *
 * @return A RTypeMember representing the type of the argument at index \a index
 * @memberof RArrayType
 */
RTypeMember * RUMINATE_EXPORT r_array_type_member_at(
	RArrayType *type /** [in] the type to get the member type of */,
	size_t index /** [in] the index in the array to get the member type of */,
	GError **error /** [in,out] see errors.h */
) RUMINATE_NOEXCEPT;

G_END_DECLS
