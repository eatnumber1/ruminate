/** Record members
 *
 * A RRecordMember represents a member of an aggregate type.
 *
 * @file record_member.h
 * @see RRecordMember
 */

/** An identifier denoting the real type of this RRecordMember.
 *
 * This identifier can be retrieved using r_record_member_id().
 *
 * @enum RRecordMemberId
 */
typedef enum RRecordMemberId {
	R_RECORD_MEMBER_BITFIELD,
	R_RECORD_MEMBER_OTHER
} RRecordMemberId;

/** An opaque struct representing a record member.
 *
 * @struct RRecordMember
 * @extends RTypeMember
 */
typedef struct RRecordMember RRecordMember;

G_BEGIN_DECLS

RRecordMemberId RUMINATE_EXPORT r_record_member_id( RRecordMember *, GError ** ) RUMINATE_NOEXCEPT;
RString * RUMINATE_EXPORT r_record_member_name( RRecordMember *, GError ** ) RUMINATE_NOEXCEPT;

G_END_DECLS
