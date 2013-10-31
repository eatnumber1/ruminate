/** Built-in types
 *
 * A RBuintinType represents a built-in type (`int`, `double`, etc.)
 *
 * @file builtin_type.h
 * @see RBuiltinType
 */

/** An identifier denoting the real type of this RBuiltinType.
 *
 * This identifier can be retrieved using r_builtin_type_id().
 *
 * @enum RBuiltinTypeId
 */
typedef enum RBuiltinTypeId {
	R_BUILTIN_TYPE_INT /** an int */,
	R_BUILTIN_TYPE_LONG /** a long */,
	R_BUILTIN_TYPE_DOUBLE /** a double */,
	R_BUILTIN_TYPE_SHORT /** a short */,
	R_BUILTIN_TYPE_CHAR /** a char */,
	R_BUILTIN_TYPE_VOID /** the void type */,
	R_BUILTIN_TYPE_BOOL /** a bool */,
	R_BUILTIN_TYPE_UNKNOWN /** an unknown type */
} RBuiltinTypeId;

/** An opaque struct representing a builtin type.
 *
 * @struct RBuiltinType
 * @extends RType
 */
typedef struct RBuiltinType RBuiltinType;

G_BEGIN_DECLS

/** Get the builtin type identifier of this builtin type.
 *
 * The RBuiltinTypeId of this RBuiltinType represents the real type of this
 * builtin type.
 *
 * @return the real type of this builtin type
 * @memberof RBuiltinType
 */
RBuiltinTypeId RUMINATE_EXPORT r_builtin_type_id(
	RBuiltinType *type /** [in] the builtin type to retrieve the id of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Determine if this type is signed.
 *
 * Note that the `char` type can be neither signed nor unsigned.
 *
 * @memberof RBuiltinType
 * @return whether or not this type is signed
 */
bool RUMINATE_EXPORT r_builtin_type_is_signed(
	RBuiltinType *type /** [in] the type to determine the signedness of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Determine if this type is unsigned.
 *
 * Note that the `char` type can be neither signed nor unsigned.
 *
 * @memberof RBuiltinType
 * @return whether or not this type is unsigned
 */
bool RUMINATE_EXPORT r_builtin_type_is_unsigned(
	RBuiltinType *type /** [in] the type to determine the signedness of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

G_END_DECLS
