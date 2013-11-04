/** Type members
 *
 * A RTypeMember represents a member of an array (RArrayType) or aggregate
 * (RAggregateType) type.
 *
 * @file type_member.h
 * @see RTypeMember
 */

/** An identifier denoting the child type of this RTypeMember.
 *
 * This identifier can be retrieved using r_type_member_id().
 *
 * @enum RTypeMemberId
 */
typedef enum RTypeMemberId {
	R_TYPE_MEMBER_AGGREGATE /** a RAggregateMember @see RAggregateMember */,
	R_TYPE_MEMBER_ARRAY /** an array member */
} RTypeMemberId;

/** An opaque struct representing a type member.
 *
 * @struct RTypeMember
 */
typedef struct RTypeMember RTypeMember;

G_BEGIN_DECLS

/** Get the type member id of this RTypeMember
 *
 * @memberof RTypeMember
 */
RTypeMemberId RUMINATE_EXPORT r_type_member_id(
	RTypeMember *member /** [in] the type member to get the id of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get the type of this type member.
 *
 * @memberof RTypeMember
 */
RType * RUMINATE_EXPORT r_type_member_type(
	RTypeMember *member /** [in] the type member to get the type of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get the offset of this type member into it's container.
 *
 * This is the number of bytes into this type member's container (either an
 * RArrayType, or an RAggregateType) that this member is located.
 *
 * @memberof RTypeMember
 * @see RArrayType
 * @see RAggregateType
 */
ptrdiff_t RUMINATE_EXPORT r_type_member_offset(
	RTypeMember *member /** [in] the type member to get the offset of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Increment the reference count of this RTypeMember.
 *
 * @memberof RTypeMember
 */
RTypeMember * RUMINATE_EXPORT r_type_member_ref(
	RTypeMember *member /** [in] the type member to increment the reference count of */
) RUMINATE_NOEXCEPT;

/** Decrement the reference count of this RTypeMember.
 *
 * If the reference count reaches zero, this RTypeMember will be freed.
 *
 * @memberof RTypeMember
 */
void RUMINATE_EXPORT r_type_member_unref(
	RTypeMember *member /** [in] the type member to decrement the reference count of */
) RUMINATE_NOEXCEPT;

G_END_DECLS
