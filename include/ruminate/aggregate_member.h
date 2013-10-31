/** Aggregate members
 *
 * A RAggregateMember represents a member of an aggregate type.
 *
 * @file aggregate_member.h
 * @see RAggregateMember
 * @see RAggregateType
 */

/** An identifier denoting the real type of this RAggregateMember.
 *
 * This identifier can be retrieved using r_aggregate_member_id().
 *
 * @enum RAggregateMemberId
 */
typedef enum RAggregateMemberId {
	R_AGGREGATE_MEMBER_BITFIELD /** a bitfield */,
	R_AGGREGATE_MEMBER_ENUM /** an instance of REnumMember @see REnumMember */,
	R_AGGREGATE_MEMBER_OTHER /** a "normal" type (non enum-member nor bitfield) */
} RAggregateMemberId;

/** An opaque struct representing a aggregate member.
 *
 * @struct RAggregateMember
 * @extends RTypeMember
 * @see RAggregateType
 */
typedef struct RAggregateMember RAggregateMember;

G_BEGIN_DECLS

/** Get the real type identifier of this aggregate member.
 *
 * @memberof RAggregateMember
 * @return the real type of this aggregate member
 */
RAggregateMemberId RUMINATE_EXPORT r_aggregate_member_id(
	RAggregateMember *member /** [in] the aggregate member to get the id of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get the name of this aggregate member
 *
 * @memberof RAggregateMember
 * @return a RString containing the name of this aggregate member
 * @todo Function argument names return `""`. I'm not sure it's even possible to
 *       get these, so this feature might go away.
 */
RString * RUMINATE_EXPORT r_aggregate_member_name(
	RAggregateMember *member /** [in] the aggregate member to get the name of */,
	GError **error /** [in] see errors.h */
) RUMINATE_NOEXCEPT;

G_END_DECLS
