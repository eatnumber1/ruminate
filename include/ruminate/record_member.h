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

/** Get the real type identifier of this record member.
 *
 * @memberof RRecordMember
 * @return the real type of this record member
 */
RRecordMemberId RUMINATE_EXPORT r_record_member_id(
	RRecordMember *member /** [in] the record member to get the id of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get the name of this record member
 *
 * @memberof RRecordMember
 * @return a RString containing the name of this record member
 */
RString * RUMINATE_EXPORT r_record_member_name(
	RRecordMember *member /** [in] the record member to get the name of */,
	GError **error /** [in] see errors.h */
) RUMINATE_NOEXCEPT;

G_END_DECLS
