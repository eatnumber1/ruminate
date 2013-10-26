/** Aggregate types
 *
 * A RAggregateType represents an aggregate type (`struct`, `union`, `enum` or `function`)
 *
 * @file aggregate_type.h
 * @see RAggregateType
 */

/** An identifier denoting the child type of this RAggregateType.
 *
 * This identifier can be retrieved using r_aggregate_type_id().
 *
 * @enum RAggregateTypeId
 */
typedef enum RAggregateTypeId {
	R_AGGREGATE_TYPE_TAG /** a tagged type @see RTagType */,
	R_AGGREGATE_TYPE_FUNCTION /** a function type @see RFunctionType */
} RAggregateTypeId;

/** An opaque struct representing a aggregate type.
 *
 * This aggregate type can be safely cast to it's sub-type which can be determined
 * by using r_aggregate_type_id()
 *
 * @struct RAggregateType
 * @extends RType
 */
typedef struct RAggregateType RAggregateType;

G_BEGIN_DECLS

/** Get the aggregate type identifier of this aggregate type.
 *
 * The RAggregateTypeId of this RAggregateType represents the child type of this
 * RAggregateType, and can be safely cast into that child type.
 *
 * @return the child type of this aggregate type.
 * @memberof RAggregateType
 */
RAggregateTypeId RUMINATE_EXPORT r_aggregate_type_id(
	RAggregateType *type /** [in] the aggregate type to retrieve the id of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get the number of members in this aggregate type.
 *
 * @return the number of members in this aggregate type.
 * @memberof RAggregateType
 */
size_t RUMINATE_EXPORT r_aggregate_type_nmembers(
	RAggregateType *type /** [in] the type to get the number of members of */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

/** Get a aggregate's member at a specified index.
 *
 * @return a RAggregateMember representing the member of this aggregate at index \a
 *         index.
 * @memberof RAggregateType
 */
RAggregateMember * RUMINATE_EXPORT r_aggregate_type_member_at(
	RAggregateType *type /** [in] the aggregate type to retrieve a member of */,
	size_t index /** [in] the index of the member */,
	GError **error /** [out] see errors.h */
) RUMINATE_NOEXCEPT;

G_END_DECLS
