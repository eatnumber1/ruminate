/** The top level of the ruminate type hierarchy.
 *
 * @file type.h
 * @see RType
 */

/** An identifier denoting the child type of this RType.
 *
 * This identifier can be retrieved using r_type_id().
 *
 * @enum RTypeId
 */
typedef enum RTypeId {
	R_TYPE_BUILTIN /** a builtin type @see RBuiltinType */,
	R_TYPE_AGGREGATE /** an aggregate type @see RAggregateType */,
	R_TYPE_TYPEDEF /** a typedef @see RTypedefType */,
	R_TYPE_POINTER /** a pointer @see RPointerType */,
	R_TYPE_ARRAY /** an array @see RArrayType */,
	R_TYPE_UNKNOWN /** an unknown type */
} RTypeId;

/** An opaque struct representing a type.
 *
 * This type can be safely cast to it's sub-type which can be determined using
 * r_type_id().
 *
 * @struct RType
 */
typedef struct RType RType;

G_BEGIN_DECLS

/** Get the type identifier of this type.
 *
 * The RTypeId of this RType represents the child type of this RType, and can be
 * safely cast into that child type.
 *
 * @return the child type of this type.
 * @memberof RType
 */
RTypeId RUMINATE_EXPORT r_type_id(
	RType *type /** [in] the type to retrieve the type id of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get the name of this type.
 * 
 * @return an RString containing the name of this type.
 * @memberof RType
 */
RString * RUMINATE_EXPORT r_type_name(
	RType *type /** [in] the type to retrieve the name of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Increase the reference count of this type
 *
 * @return \a type
 * @memberof RType
 */
RType * RUMINATE_EXPORT r_type_ref(
	RType *type /** [in] the type to increase the reference count of */
) RUMINATE_NOEXCEPT;

/** Decrease the reference count of this
 *
 * This RType will be freed if the reference count drops to zero.
 *
 * @memberof RType
 */
void RUMINATE_EXPORT r_type_unref(
	RType *type /** [in] the type to decrease the reference count of */
) RUMINATE_NOEXCEPT;

/** Get an RType representing a pointer to this type
 *
 * @return An RType representing a pointer to \a type.
 * @memberof RType
 */
RType * RUMINATE_EXPORT r_type_pointer(
	RType *type /** [in] the type to get a pointer to */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

G_END_DECLS
