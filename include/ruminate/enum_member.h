/** An opaque struct representing an enum member.
 *
 * @struct REnumMember
 * @extends RAggregateMember
 */
typedef struct REnumMember REnumMember;

G_BEGIN_DECLS

/** Get the signed value of this enum member.
 *
 * @memberof REnumMember
 * @return the signed value of this enum member
 */
intmax_t RUMINATE_EXPORT r_enum_member_value_signed(
	REnumMember *member /** [in] the enum member to get the value of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get the unsigned value of this enum member.
 *
 * @memberof REnumMember
 * @return the unsigned value of this enum member
 */
uintmax_t RUMINATE_EXPORT r_enum_member_value_unsigned(
	REnumMember *member /** [in] the enum member to get the value of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

G_END_DECLS
