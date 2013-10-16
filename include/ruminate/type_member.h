/** Type members
 *
 * A RTypeMember represents a member of an array (RArrayType) or record
 * (RRecordType) type.
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
	R_TYPE_MEMBER_RECORD /** a RRecordMember @see RRecordMember */,
	R_TYPE_MEMBER_ARRAY /** an array member */
} RTypeMemberId;

/** An opaque struct representing a type member.
 *
 * @struct RTypeMember
 */
typedef struct RTypeMember RTypeMember;

G_BEGIN_DECLS

RTypeMemberId RUMINATE_EXPORT r_type_member_id( RTypeMember *, GError ** ) RUMINATE_NOEXCEPT;
RType * RUMINATE_EXPORT r_type_member_type( RTypeMember *, GError ** ) RUMINATE_NOEXCEPT;
off_t RUMINATE_EXPORT r_type_member_offset( RTypeMember *, GError ** ) RUMINATE_NOEXCEPT;

RTypeMember * RUMINATE_EXPORT r_type_member_ref( RTypeMember * ) RUMINATE_NOEXCEPT;
void RUMINATE_EXPORT r_type_member_unref( RTypeMember * ) RUMINATE_NOEXCEPT;

G_END_DECLS
