/** Record types
 *
 * A RRecordType represents an aggregate type (`struct`, `union` or `enum`)
 *
 * @file record_type.h
 * @see RRecordType
 */

/** An identifier denoting the child type of this RRecordType.
 *
 * This identifier can be retrieved using r_record_type_id().
 *
 * @enum RRecordTypeId
 */
typedef enum RRecordTypeId {
	R_RECORD_TYPE_TAG /** a tagged type @see RTagType */,
	R_RECORD_TYPE_FUNCTION /** a function type @see RFunctionType */
} RRecordTypeId;

/** An opaque struct representing a record type.
 *
 * This record type can be safely cast to it's sub-type which can be determined
 * by using r_record_type_id()
 *
 * @struct RRecordType
 * @extends RType
 */
typedef struct RRecordType RRecordType;

G_BEGIN_DECLS

/** Get the record type identifier of this record type.
 *
 * The RRecordTypeId of this RRecordType represents the child type of this
 * RRecordType, and can be safely cast into that child type.
 *
 * @return the child type of this record type.
 * @memberof RRecordType
 */
RRecordTypeId RUMINATE_EXPORT r_record_type_id(
	RRecordType *type /** [in] the record type to retrieve the id of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get the number of members in this record type.
 *
 * @return the number of members in this record type.
 * @memberof RRecordType
 */
size_t RUMINATE_EXPORT r_record_type_nmembers(
	RRecordType *type /** [in] the type to get the number of members of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get a record's member at a specified index.
 *
 * @return a RRecordMember representing the member of this record at index \a
 *         index.
 * @memberof RRecordType
 */
RRecordMember * RUMINATE_EXPORT r_record_type_member_at(
	RRecordType *type /** [in] the record type to retrieve a member of */,
	size_t index /** [in] the index of the member */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

G_END_DECLS
