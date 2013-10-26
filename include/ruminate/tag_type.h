/** Tagged types
 *
 * A RTagType represents a tagged type (`struct`, `union` or `enum`)
 * 
 * A type's tag is the name that comes after it's declaration's qualifier (e.x.
 * `struct foo`'s tag is `foo`)
 *
 * @file tag_type.h
 * @see RTagType
 */

/** An identifier denoting the real type of this RTaggedType, one of `struct`,
 * `union` or `enum`.
 *
 * This identifier can be retrieved using r_tag_type_id().
 *
 * @enum RTagTypeId
 */
typedef enum RTagTypeId {
	R_TAG_TYPE_STRUCTURE /** a `struct` */,
	R_TAG_TYPE_UNION /** a `union` */,
	R_TAG_TYPE_ENUM /** an `enum` */
} RTagTypeId;

/** An opaque struct representing a tagged type.
 *
 * @struct RTagType
 * @extends RAggregateType
 */
typedef struct RTagType RTagType;

G_BEGIN_DECLS

/** Get the tag type identifier of this tagged type.
 *
 * The RTagTypeId of this RTagType represents the real type of this tagged type,
 * one of `struct`, `union` or `enum`.
 *
 * @return the real typ eof this tagged type
 * @memberof RTagType
 */
RTagTypeId RUMINATE_EXPORT r_tag_type_id(
	RTagType *type /** [in] the tagged type to retrieve the id of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get the tag name of this tagged type.
 *
 * @todo This function is currently unimplemented, and will always return
 *       failure.
 * @memberof RTagType
 */
RString * RUMINATE_EXPORT r_tag_type_name(
	RTagType *type /** [in] the type to get the tag name of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

G_END_DECLS
